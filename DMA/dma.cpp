#include "dma.h"
#include "../common/soc_map.h"
#include <iostream>
#include <cstring>

DMA::DMA(sc_module_name name)
: sc_module(name),
  mem_socket("mem_socket"),
  target_socket("target_socket")
{
    target_socket.register_b_transport(this, &DMA::b_transport);
    SC_THREAD(run);
    
    src_addr = 0;
    dst_addr = 0;
    length   = 0;
    ctrl     = 0;
}

void DMA::run()
{
    while (true) {
        wait(start_event);
        std::cout << sc_time_stamp() << " DMA: transfer started from 0x" << std::hex << src_addr 
                  << " to 0x" << dst_addr << " len " << std::dec << length << "\n";

        tlm::tlm_generic_payload trans;
        sc_time delay = SC_ZERO_TIME;
        
        // Simulating data movement: Read from SRC, Write to DST
        // (Simplified: just write a payload to DST to verify bus activity)
        static uint32_t dma_payload = 0xDEADBEEF;

        trans.set_command(tlm::TLM_WRITE_COMMAND);
        trans.set_address(dst_addr);
        trans.set_data_ptr((unsigned char*)&dma_payload);
        trans.set_data_length(4);
        trans.set_streaming_width(4);
        trans.set_byte_enable_ptr(nullptr);
        trans.set_dmi_allowed(false);
        trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

        mem_socket->b_transport(trans, delay);
        wait(delay);

        if (trans.get_response_status() == tlm::TLM_OK_RESPONSE) {
            std::cout << sc_time_stamp() << " DMA: transfer complete\n";
        } else {
            std::cerr << sc_time_stamp() << " DMA ERROR: " << trans.get_response_string() << "\n";
        }

        // Reset control register
        ctrl = 0;

        // Completion interrupt pulse
        irq_out.write(true);
        wait(10, SC_NS);
        irq_out.write(false);
    }
}

void DMA::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
{
    uint64_t addr = trans.get_address();
    unsigned char* ptr = trans.get_data_ptr();

    if (trans.is_write()) {
        uint32_t val = *((uint32_t*)ptr);
        switch (addr) {
            case DMA_REG_SRC:  src_addr = val; break;
            case DMA_REG_DST:  dst_addr = val; break;
            case DMA_REG_LEN:  length   = val; break;
            case DMA_REG_CTRL: 
                ctrl = val;
                if (ctrl & 0x1) start_event.notify(delay);
                break;
        }
    } else {
        uint32_t val = 0;
        switch (addr) {
            case DMA_REG_SRC:  val = src_addr; break;
            case DMA_REG_DST:  val = dst_addr; break;
            case DMA_REG_LEN:  val = length;   break;
            case DMA_REG_CTRL: val = ctrl;     break;
        }
        *((uint32_t*)ptr) = val;
    }
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
}