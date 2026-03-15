#ifndef DMA_H
#define DMA_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

using namespace sc_core;

struct DMA : sc_module {

    // Initiator socket: DMA uses this to write data to DRAM via the AXI bus
    tlm_utils::simple_initiator_socket<DMA> mem_socket;
    // Target socket: CPU uses this to trigger DMA transfers
    tlm_utils::simple_target_socket<DMA> target_socket;

    sc_out<bool> irq_out;

    SC_CTOR(DMA);

    // Active DMA transfer thread — auto-starts at simulation time
    void run();

    virtual void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay);

private:
    sc_event trig_event;
};

#endif