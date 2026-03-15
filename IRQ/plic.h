#ifndef PLIC_H
#define PLIC_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include "../common/soc_map.h"

using namespace sc_core;

struct PLIC : sc_module {

    tlm_utils::simple_target_socket<PLIC> socket;

    sc_in<bool> nvdla_irq;
    sc_in<bool> dma_irq;
    sc_out<bool> cpu_irq;
    
    SC_CTOR(PLIC);

    void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay);
    void update_irq();

private:
    uint32_t regs[4]; // Pending, Enable, Claim, Complete
    sc_event e_update_irq;
};

#endif
