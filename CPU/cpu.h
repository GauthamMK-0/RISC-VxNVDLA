#ifndef CPU_H
#define CPU_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>

#include <riscv/sim.h>
#include <riscv/decode.h>
#include <riscv/devices.h>

using namespace sc_core;

class SystemCBridge : public abstract_device_t {
public:
    SystemCBridge(struct CPU* cpu, tlm_utils::simple_initiator_socket<struct CPU>& socket, reg_t size, reg_t base);
    bool load(reg_t addr, size_t len, uint8_t* bytes) override;
    bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
    reg_t size() override { return sz; }

private:
    struct CPU* cpu_mod;
    tlm_utils::simple_initiator_socket<struct CPU>& socket;
    reg_t sz;
    reg_t base_addr;
};

struct CPU : sc_module {

    tlm_utils::simple_initiator_socket<CPU> socket;
    sc_in<bool> irq_in;

    SC_HAS_PROCESS(CPU);
    CPU(sc_module_name name, const char* elf_path = nullptr);
    ~CPU();

    void run();
    void check_response(tlm::tlm_generic_payload& trans);

    // Spike objects
    cfg_t* spike_cfg;
    sim_t* spike_sim;
    SystemCBridge* bridge_dram;
    SystemCBridge* bridge_mmio;
    const char* elf_file;
};

#endif