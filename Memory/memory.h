#ifndef MEMORY_H
#define MEMORY_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>

using namespace sc_core;

struct MEMORY : sc_module {

    tlm_utils::simple_target_socket<MEMORY> socket;

    unsigned char* mem;

    SC_CTOR(MEMORY);
    ~MEMORY();

    void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay);
};

#endif