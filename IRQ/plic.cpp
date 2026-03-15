#include "plic.h"
#include <iostream>

PLIC::PLIC(sc_module_name name)
: sc_module(name), socket("socket")
{
    socket.register_b_transport(this, &PLIC::b_transport);
    
    SC_METHOD(update_irq);
    sensitive << nvdla_irq.pos() << dma_irq.pos() << e_update_irq;
    
    for(int i=0; i<4; i++) regs[i] = 0;
}

void PLIC::update_irq()
{
    // Update pending bits from hardware lines
    if (nvdla_irq.posedge()) regs[0] |= (1 << (IRQ_ID_NVDLA - 1));
    if (dma_irq.posedge())   regs[0] |= (1 << (IRQ_ID_DMA - 1));
    
    // Evaluate IRQ output: Pending AND Enabled
    bool any_pending_enabled = false;
    for (uint32_t id = 1; id <= 2; id++) {
        if ((regs[0] & (1 << (id - 1))) && (regs[1] & (1 << (id - 1)))) {
            any_pending_enabled = true;
            break;
        }
    }
    cpu_irq.write(any_pending_enabled);
}

void PLIC::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
{
    uint64_t addr = trans.get_address();
    unsigned char* ptr = trans.get_data_ptr();
    bool need_update = false;

    if (trans.is_write()) {
        uint32_t val = *((uint32_t*)ptr);
        if (addr == PLIC_REG_ENABLE) {
            regs[1] = val; // Enable
            need_update = true;
        } else if (addr == PLIC_REG_COMPLETE) {
            uint32_t irq_id = val;
            if (irq_id > 0 && irq_id <= 2) {
                regs[0] &= ~(1 << (irq_id - 1)); // Clear pending bit
                std::cout << sc_time_stamp() << " PLIC: IRQ " << irq_id << " completed\n";
                need_update = true;
            }
        }
    } else {
        uint32_t val = 0;
        if (addr == PLIC_REG_SOURCE) {
            val = regs[0];
        } else if (addr == PLIC_REG_ENABLE) {
            val = regs[1];
        } else if (addr == PLIC_REG_CLAIM) {
            for (uint32_t id = 1; id <= 2; id++) {
                if ((regs[0] & (1 << (id - 1))) && (regs[1] & (1 << (id - 1)))) {
                    val = id;
                    break;
                }
            }
            std::cout << sc_time_stamp() << " PLIC: IRQ " << val << " claimed\n";
        }
        *((uint32_t*)ptr) = val;
    }
    
    if (need_update) {
        e_update_irq.notify(delay);
    }

    trans.set_response_status(tlm::TLM_OK_RESPONSE);
}
