#include "nvdla.h"

void NVDLA::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
{
    uint64_t addr = trans.get_address();
    unsigned char* ptr = trans.get_data_ptr();

    if (trans.is_write()) {
        uint32_t val = *((uint32_t*)ptr);
        regs[addr >> 2] = val;

        if (addr == NVDLA_REG_START && (val & 0x1)) {
            start_event.notify(delay);
        } else if (addr == NVDLA_REG_BDMA_CTRL && (val & 0x1)) {
            bdma_start_event.notify(delay);
        }
    } else {
        *((uint32_t*)ptr) = regs[addr >> 2];
    }

    trans.set_response_status(tlm::TLM_OK_RESPONSE);
}
