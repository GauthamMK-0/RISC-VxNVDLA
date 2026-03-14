#ifndef NVDLA_H
#define NVDLA_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>

using namespace sc_core;

struct NVDLA : sc_module {

    tlm_utils::simple_target_socket<NVDLA> socket;

    sc_out<bool> irq;   // interrupt output to IRQ controller

    uint32_t ctrl;
    uint32_t status;

    SC_CTOR(NVDLA);

    void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay);
    void compute();
};

#endif