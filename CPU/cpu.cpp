#include "cpu.h"
#include "../common/soc_map.h"
#include <iostream>
#include <vector>
#include <string>

SystemCBridge::SystemCBridge(CPU* cpu, tlm_utils::simple_initiator_socket<struct CPU>& s, reg_t size, reg_t base) 
: cpu_mod(cpu), socket(s), sz(size), base_addr(base) {}

bool SystemCBridge::load(reg_t addr, size_t len, uint8_t* bytes) {
    tlm::tlm_generic_payload trans;
    sc_time delay = SC_ZERO_TIME;

    trans.set_command(tlm::TLM_READ_COMMAND);
    trans.set_address(addr + base_addr);
    trans.set_data_ptr(bytes);
    trans.set_data_length(len);
    trans.set_streaming_width(len);
    trans.set_byte_enable_ptr(0);
    trans.set_dmi_allowed(false);
    trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

    socket->b_transport(trans, delay);
    
    // Advance SystemC time
    wait(delay);
    
    return trans.get_response_status() == tlm::TLM_OK_RESPONSE;
}

bool SystemCBridge::store(reg_t addr, size_t len, const uint8_t* bytes) {
    tlm::tlm_generic_payload trans;
    sc_time delay = SC_ZERO_TIME;

    trans.set_command(tlm::TLM_WRITE_COMMAND);
    trans.set_address(addr + base_addr);
    trans.set_data_ptr(const_cast<uint8_t*>(bytes));
    trans.set_data_length(len);
    trans.set_streaming_width(len);
    trans.set_byte_enable_ptr(0);
    trans.set_dmi_allowed(false);
    trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

    socket->b_transport(trans, delay);
    
    // Advance SystemC time
    wait(delay);

    return trans.get_response_status() == tlm::TLM_OK_RESPONSE;
}

CPU::CPU(sc_module_name name, const char* elf_path)
: sc_module(name), socket("socket"), elf_file(elf_path)
{
    std::cout << "CPU Constructor: elf_file=" << (elf_file ? elf_file : "NULL") << std::endl;
    SC_THREAD(run);
    // Separate bridges to avoid Spike's internal boot ROM [0x1000, 0x2000) and CLINT [0x2000000, 0x20c0000)
    bridge_dram = new SystemCBridge(this, socket, 0x02000000ULL - 0x2000ULL, 0x2000ULL);
    bridge_mmio = new SystemCBridge(this, socket, 0x000300000ULL, 0x100000000ULL);
}

CPU::~CPU() {
    delete spike_sim;
    delete spike_cfg;
    delete bridge_dram;
    delete bridge_mmio;
}

void CPU::run()
{
    std::cout << sc_time_stamp() << " CPU: Thread started" << std::endl;
    if (!elf_file) {
        std::cerr << "CPU: No ELF file specified!" << std::endl;
        return;
    }

    wait(10, SC_NS);

    std::cout << sc_time_stamp() << " CPU: Initializing Spike with " << elf_file << "..." << std::endl;

    std::vector<std::string> spike_args;
    spike_args.push_back(elf_file);

    spike_cfg = new cfg_t();
    spike_cfg->isa = "RV64GC";
    spike_cfg->priv = "MSU";
    spike_cfg->hartids = {0};
    
    std::vector<std::pair<reg_t, abstract_mem_t*>> mems;
    std::vector<device_factory_sargs_t> plugin_devices;
    
    spike_sim = new sim_t(
        spike_cfg, false, mems, plugin_devices,
        false, spike_args, debug_module_config_t(),
        nullptr, true, nullptr, false, nullptr, std::nullopt
    );

    // Add bridges
    spike_sim->add_device(0x2000, std::shared_ptr<abstract_device_t>(bridge_dram, [](SystemCBridge*){}));
    spike_sim->add_device(0x100000000ULL, std::shared_ptr<abstract_device_t>(bridge_mmio, [](SystemCBridge*){}));

    // Start simulation
    std::cout << sc_time_stamp() << " CPU: Starting Spike execution..." << std::endl;
    
    try {
        spike_sim->run();
    } catch (const std::exception& e) {
        std::cerr << sc_time_stamp() << " CPU: Spike Exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << sc_time_stamp() << " CPU: Spike Unknown Exception" << std::endl;
    }
    
    std::cout << sc_time_stamp() << " CPU: Spike simulation finished." << std::endl;
}

void CPU::check_response(tlm::tlm_generic_payload& trans)
{
    if (trans.get_response_status() != tlm::TLM_OK_RESPONSE) {
        std::cerr << sc_time_stamp() << " CPU TLM ERROR: " 
                  << trans.get_response_string() << "\n";
    }
}