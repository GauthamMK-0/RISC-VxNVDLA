#include "cpu.h"
#include <iostream>

CPU::CPU(sc_module_name name)
: sc_module(name), socket("socket")
{
    SC_THREAD(run);
}

void CPU::run()
{
    wait(10, SC_NS);

    tlm::tlm_generic_payload trans;
    sc_time delay = SC_ZERO_TIME;
    uint32_t data = 1;

    // 1. Kick NVDLA
    std::cout << sc_time_stamp() << " CPU kicking NVDLA...\n";
    trans.set_command(tlm::TLM_WRITE_COMMAND);
    trans.set_address(0x40000000); // NVDLA Base
    trans.set_data_ptr((unsigned char*)&data);
    trans.set_data_length(4);
    socket->b_transport(trans, delay);
    wait(delay);

    // 2. Wait for NVDLA Interrupt
    std::cout << sc_time_stamp() << " CPU waiting for NVDLA IRQ...\n";
    if (!irq_in.read()) {
        wait(irq_in.posedge_event());
    }
    std::cout << sc_time_stamp() << " CPU received NVDLA IRQ\n";

    // 3. Kick DMA
    std::cout << sc_time_stamp() << " CPU kicking DMA...\n";
    delay = SC_ZERO_TIME;
    trans.set_command(tlm::TLM_WRITE_COMMAND);
    trans.set_address(0x50000000); // DMA Base (Assumed)
    trans.set_data_ptr((unsigned char*)&data);
    trans.set_data_length(4);
    socket->b_transport(trans, delay);
    wait(delay);

    // 4. Wait for DMA Interrupt
    std::cout << sc_time_stamp() << " CPU waiting for DMA IRQ...\n";
    wait(irq_in.posedge_event());
    std::cout << sc_time_stamp() << " CPU received DMA IRQ. Task finished.\n";
}