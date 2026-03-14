#ifndef AXI_IC_H
#define AXI_IC_H

#include <systemc>
#include <tlm>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>

using namespace sc_core;

struct AXI_IC : sc_module {

    // Two master-facing target ports (cpu=0, dma=1)
    tlm_utils::multi_passthrough_target_socket<AXI_IC> cpu_socket;
    tlm_utils::multi_passthrough_target_socket<AXI_IC> dma_socket;

    // Slave-facing initiator ports
    tlm_utils::simple_initiator_socket<AXI_IC> mem_socket;
    tlm_utils::simple_initiator_socket<AXI_IC> nvdla_socket;

    SC_CTOR(AXI_IC);

    // multi_passthrough_target_socket uses (int id, payload, phase, delay)
    tlm::tlm_sync_enum nb_transport_fw(int id,
                                       tlm::tlm_generic_payload& trans,
                                       tlm::tlm_phase& phase,
                                       sc_time& delay);

    void b_transport(int id,
                     tlm::tlm_generic_payload& trans,
                     sc_time& delay);
};

#endif