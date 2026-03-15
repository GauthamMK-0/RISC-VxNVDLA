#include "nvdla.h"
#include "../common/soc_map.h"
#include <iostream>

NVDLA::NVDLA(sc_module_name name)
: sc_module(name),
  socket("socket"),
  irq("irq")
{
    socket.register_b_transport(this, &NVDLA::b_transport);

    ctrl   = 0;
    status = 0;
    irq.initialize(false);
}

void NVDLA::b_transport(tlm::tlm_generic_payload& trans,
                        sc_time& delay)
{
    uint64_t addr         = trans.get_address();
    unsigned char* ptr    = trans.get_data_ptr();

    // Model interconnect-to-register access latency
    delay += sc_time(20, SC_NS);

    if (trans.is_write())
    {
        uint32_t val = *((uint32_t*)ptr);

        switch (addr)
        {
            case NVDLA_REG_START:  // START register
                ctrl = val;
                if (ctrl & 0x1)
                    sc_spawn(sc_bind(&NVDLA::compute, this));
                break;

            default:
                break;
        }
    }
    else
    {
        uint32_t val = 0;

        switch (addr)
        {
            case NVDLA_REG_STATUS:  // STATUS register
                val = status;
                break;

            default:
                val = 0;
        }

        *((uint32_t*)ptr) = val;
    }

    trans.set_response_status(tlm::TLM_OK_RESPONSE);
}

void NVDLA::compute()
{
    status = 1;   // busy

    wait(500, SC_NS);   // simulate accelerator latency

    status = 0;   // idle

    std::cout << sc_time_stamp()
              << " NVDLA job finished"
              << std::endl;

    // Generate interrupt pulse
    irq.write(true);
    wait(10, SC_NS);
    irq.write(false);
}