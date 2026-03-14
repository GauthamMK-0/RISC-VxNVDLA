#ifndef DMA_H
#define DMA_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>

using namespace sc_core;

struct DMA : sc_module {

    // Initiator socket: DMA uses this to write data to DRAM via the AXI bus
    tlm_utils::simple_initiator_socket<DMA> mem_socket;

    SC_CTOR(DMA);

    // Active DMA transfer thread — auto-starts at simulation time
    void run();
};

#endif