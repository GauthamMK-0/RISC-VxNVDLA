#ifndef CPU_H
#define CPU_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>

using namespace sc_core;

struct CPU : sc_module {

    tlm_utils::simple_initiator_socket<CPU> socket;

    sc_in<bool> irq_in;

    SC_CTOR(CPU);

    void run();

private:
    uint8_t l1_scratchpad[1024]; // 1KB Local "Cache"
    
    void check_response(tlm::tlm_generic_payload& trans);
};

#endif