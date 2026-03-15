#include "cpu.h"
#include "../common/soc_map.h"
#include <iostream>

CPU::CPU(sc_module_name name)
: sc_module(name), socket("socket")
{
    SC_THREAD(run);
    for (int i = 0; i < 1024; i++) l1_scratchpad[i] = 0;
}

void CPU::run()
{
    wait(10, SC_NS);

    tlm::tlm_generic_payload trans;
    sc_time delay = SC_ZERO_TIME;
    uint32_t data;

    std::cout << sc_time_stamp() << " CPU: Booting and initializing PLIC...\n";
    
    // Enable IRQs in PLIC
    data = (1 << (IRQ_ID_NVDLA - 1)) | (1 << (IRQ_ID_DMA - 1));
    trans.set_command(tlm::TLM_WRITE_COMMAND);
    trans.set_address(PLIC_BASE + PLIC_REG_ENABLE);
    trans.set_data_ptr((unsigned char*)&data);
    trans.set_data_length(4);
    socket->b_transport(trans, delay);
    check_response(trans);
    wait(delay);

    // 1. Kick NVDLA via Register
    std::cout << sc_time_stamp() << " CPU: kicking NVDLA (Register write)...\n";
    data = 1;
    delay = SC_ZERO_TIME;
    trans.set_command(tlm::TLM_WRITE_COMMAND);
    trans.set_address(NVDLA_BASE + NVDLA_REG_START);
    trans.set_data_ptr((unsigned char*)&data);
    socket->b_transport(trans, delay);
    check_response(trans);
    wait(delay);

    // Loop for handling PLIC interrupts
    bool nvdla_done = false;
    bool dma_done = false;

    while (!(nvdla_done && dma_done)) {
        if (!irq_in.read()) {
            wait(irq_in.posedge_event());
        }

        // Claim IRQ from PLIC
        uint32_t irq_id = 0;
        delay = SC_ZERO_TIME;
        trans.set_command(tlm::TLM_READ_COMMAND);
        trans.set_address(PLIC_BASE + PLIC_REG_CLAIM);
        trans.set_data_ptr((unsigned char*)&irq_id);
        socket->b_transport(trans, delay);
        check_response(trans);
        wait(delay);

        if (irq_id == IRQ_ID_NVDLA) {
            std::cout << sc_time_stamp() << " CPU: Handling NVDLA Interrupt\n";
            nvdla_done = true;

            // Kick DMA (Register-level configuration)
            std::cout << sc_time_stamp() << " CPU: Programming DMA (SRC=0x1000, DST=0x2000, LEN=4)...\n";
            
            uint32_t src = 0x1000;
            uint32_t dst = 0x2000;
            uint32_t len = 4;
            uint32_t ctrl = 1; // Start

            // Set SRC
            trans.set_command(tlm::TLM_WRITE_COMMAND);
            trans.set_address(DMA_BASE + DMA_REG_SRC);
            trans.set_data_ptr((unsigned char*)&src);
            socket->b_transport(trans, delay);
            wait(delay);

            // Set DST
            trans.set_address(DMA_BASE + DMA_REG_DST);
            trans.set_data_ptr((unsigned char*)&dst);
            socket->b_transport(trans, delay);
            wait(delay);

            // Set LEN
            trans.set_address(DMA_BASE + DMA_REG_LEN);
            trans.set_data_ptr((unsigned char*)&len);
            socket->b_transport(trans, delay);
            wait(delay);

            // Set CTRL (Start)
            trans.set_address(DMA_BASE + DMA_REG_CTRL);
            trans.set_data_ptr((unsigned char*)&ctrl);
            socket->b_transport(trans, delay);
            wait(delay);

        } else if (irq_id == IRQ_ID_DMA) {
            std::cout << sc_time_stamp() << " CPU: Handling DMA Interrupt\n";
            dma_done = true;
        }

        // Complete IRQ in PLIC
        if (irq_id != 0) {
            delay = SC_ZERO_TIME;
            trans.set_command(tlm::TLM_WRITE_COMMAND);
            trans.set_address(PLIC_BASE + PLIC_REG_COMPLETE);
            trans.set_data_ptr((unsigned char*)&irq_id);
            socket->b_transport(trans, delay);
            check_response(trans);
            wait(delay);
        }
    }

    std::cout << sc_time_stamp() << " CPU: All tasks finished via PLIC handshaking.\n";
}

void CPU::check_response(tlm::tlm_generic_payload& trans)
{
    if (trans.get_response_status() != tlm::TLM_OK_RESPONSE) {
        std::cerr << sc_time_stamp() << " CPU TLM ERROR: " 
                  << trans.get_response_string() << "\n";
    }
}