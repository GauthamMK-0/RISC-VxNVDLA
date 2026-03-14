#include "dma.h"
#include <iostream>
#include <cstring>

DMA::DMA(sc_module_name name)
: sc_module(name),
  mem_socket("mem_socket")
{
    SC_THREAD(run);
}

// Active DMA transfer: waits ~200 ns then writes to DRAM
void DMA::run()
{
    // Wait until ~200 ns into simulation before starting transfer
    wait(200, SC_NS);

    static uint32_t dma_payload = 0xDEADBEEF;

    tlm::tlm_generic_payload trans;
    sc_time delay = SC_ZERO_TIME;

    trans.set_command(tlm::TLM_WRITE_COMMAND);
    trans.set_address(0x0);                        // DRAM base address
    trans.set_data_ptr((unsigned char*)&dma_payload);
    trans.set_data_length(4);
    trans.set_streaming_width(4);
    trans.set_byte_enable_ptr(nullptr);
    trans.set_dmi_allowed(false);
    trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

    mem_socket->b_transport(trans, delay);
    wait(delay);   // consume annotated DRAM latency (50 ns)

    if (trans.get_response_status() == tlm::TLM_OK_RESPONSE)
        std::cout << sc_time_stamp() << " DMA wrote memory\n";
    else
        std::cout << sc_time_stamp() << " DMA ERROR: bad response\n";
}