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

    uint32_t start = 1;

    trans.set_command(tlm::TLM_WRITE_COMMAND);
    trans.set_address(0x40000000);
    trans.set_data_ptr((unsigned char*)&start);
    trans.set_data_length(4);

    socket->b_transport(trans, delay);

    wait(delay);

    std::cout << sc_time_stamp() << " CPU started NVDLA\n";
}