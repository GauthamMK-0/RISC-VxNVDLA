#ifndef NVDLA_H
#define NVDLA_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include "../common/soc_map.h"

using namespace sc_core;

struct NVDLA : sc_module {

    // Interfaces
    tlm_utils::simple_target_socket<NVDLA>    socket; // CSB (Configuration Space Bus)
    tlm_utils::simple_initiator_socket<NVDLA> dbb_socket; // DBB (Data Backbone)
    sc_out<bool> irq;

    SC_CTOR(NVDLA);

    // CSB Handler
    void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay);

private:
    // Core Components
    void scheduler_thread();
    
    // Sub-module tasks
    void run_cdma();        // Convolution Data Memory Area
    void run_compute();     // Convolution Core
    void run_writeback();   // Output Writeback
    void run_bdma();        // Bridge DMA

    // Register File
    uint32_t regs[64]; 
    sc_event start_event;
    sc_event bdma_start_event;

    // Helper for DBB access
    void mem_access(bool write, uint64_t addr, unsigned char* data, unsigned int len);
};

#endif