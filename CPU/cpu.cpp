#include "cpu.h"
#include "../common/soc_map.h"
#include <iostream>

CPU::CPU(sc_module_name name)
: sc_module(name), socket("socket")
{
    SC_THREAD(run);
    // Initialize scratchpad
    for (int i = 0; i < 1024; i++) l1_scratchpad[i] = 0;
}

void CPU::run()
{
    wait(10, SC_NS);

    tlm::tlm_generic_payload trans;
    sc_time delay = SC_ZERO_TIME;
    uint32_t data = 1;

    // --- Local Scratchpad Access ---
    std::cout << sc_time_stamp() << " CPU: initializing local scratchpad\n";
    l1_scratchpad[0] = 0xAA;
    l1_scratchpad[1] = 0xBB;
    std::cout << sc_time_stamp() << " CPU: local scratchpad[0] = " << (int)l1_scratchpad[0] << "\n";

    // 1. Kick NVDLA
    std::cout << sc_time_stamp() << " CPU: kicking NVDLA...\n";
    trans.set_command(tlm::TLM_WRITE_COMMAND);
    trans.set_address(NVDLA_BASE);
    trans.set_data_ptr((unsigned char*)&data);
    trans.set_data_length(4);
    
    socket->b_transport(trans, delay);
    check_response(trans);
    wait(delay);

    // 2. Wait for NVDLA Interrupt
    std::cout << sc_time_stamp() << " CPU: waiting for NVDLA IRQ...\n";
    if (!irq_in.read()) {
        wait(irq_in.posedge_event());
    }
    std::cout << sc_time_stamp() << " CPU: received NVDLA IRQ\n";

    // 3. Kick DMA
    std::cout << sc_time_stamp() << " CPU: kicking DMA...\n";
    delay = SC_ZERO_TIME;
    trans.set_command(tlm::TLM_WRITE_COMMAND);
    trans.set_address(DMA_BASE);
    trans.set_data_ptr((unsigned char*)&data);
    trans.set_data_length(4);
    
    socket->b_transport(trans, delay);
    check_response(trans);
    wait(delay);

    // 4. Wait for DMA Interrupt
    std::cout << sc_time_stamp() << " CPU: waiting for DMA IRQ...\n";
    wait(irq_in.posedge_event());
    std::cout << sc_time_stamp() << " CPU: received DMA IRQ. Task finished.\n";

    // --- Optional Error Injection Test ---
    /*
    std::cout << sc_time_stamp() << " CPU: Testing error handling with bad address...\n";
    trans.set_address(0xF0000000);
    socket->b_transport(trans, delay);
    check_response(trans);
    */
}

void CPU::check_response(tlm::tlm_generic_payload& trans)
{
    if (trans.get_response_status() != tlm::TLM_OK_RESPONSE) {
        std::cerr << sc_time_stamp() << " CPU TLM ERROR: " 
                  << trans.get_response_string() << "\n";
    }
}