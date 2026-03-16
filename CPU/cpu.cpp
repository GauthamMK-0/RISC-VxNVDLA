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

    // 1. Configure NVDLA for modular execution
    std::cout << sc_time_stamp() << " CPU: Configuring NVDLA engines...\n";
    
    // Set CDMA Source (weights at 0x1000)
    data = 0x1000;
    delay = SC_ZERO_TIME;
    trans.set_command(tlm::TLM_WRITE_COMMAND);
    trans.set_address(NVDLA_BASE + NVDLA_REG_CDMA_SRC);
    trans.set_data_ptr((unsigned char*)&data);
    socket->b_transport(trans, delay);
    wait(delay);

    // Set Writeback Destination (results at 0x3000)
    data = 0x3000;
    trans.set_address(NVDLA_BASE + NVDLA_REG_WR_DST);
    socket->b_transport(trans, delay);
    wait(delay);

    // Kick NVDLA
    std::cout << sc_time_stamp() << " CPU: kicking NVDLA modular pipeline...\n";
    data = 1;
    trans.set_address(NVDLA_BASE + NVDLA_REG_START);
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

            // Optional: Run BDMA in NVDLA
            std::cout << sc_time_stamp() << " CPU: Programming NVDLA BDMA (0x3000 -> 0x4000)...\n";
            uint32_t bsrc = 0x3000;
            uint32_t bdst = 0x4000;
            uint32_t blen = 4;
            uint32_t bctrl = 1;

            trans.set_command(tlm::TLM_WRITE_COMMAND);
            trans.set_address(NVDLA_BASE + NVDLA_REG_BDMA_SRC);
            trans.set_data_ptr((unsigned char*)&bsrc);
            socket->b_transport(trans, delay);
            wait(delay);

            trans.set_address(NVDLA_BASE + NVDLA_REG_BDMA_DST);
            trans.set_data_ptr((unsigned char*)&bdst);
            socket->b_transport(trans, delay);
            wait(delay);

            trans.set_address(NVDLA_BASE + NVDLA_REG_BDMA_LEN);
            trans.set_data_ptr((unsigned char*)&blen);
            socket->b_transport(trans, delay);
            wait(delay);

            trans.set_address(NVDLA_BASE + NVDLA_REG_BDMA_CTRL);
            trans.set_data_ptr((unsigned char*)&bctrl);
            socket->b_transport(trans, delay);
            wait(delay);

            // Wait for BDMA IRQ (demonstrating same ID/flow)
            std::cout << sc_time_stamp() << " CPU: Waiting for NVDLA BDMA IRQ...\n";
            wait(irq_in.posedge_event());
            
            // Claim/Complete for BDMA
            uint32_t bid = 0;
            trans.set_command(tlm::TLM_READ_COMMAND);
            trans.set_address(PLIC_BASE + PLIC_REG_CLAIM);
            trans.set_data_ptr((unsigned char*)&bid);
            socket->b_transport(trans, delay);
            std::cout << sc_time_stamp() << " CPU: BDMA IRQ " << bid << " Claimed\n";
            
            trans.set_command(tlm::TLM_WRITE_COMMAND);
            trans.set_address(PLIC_BASE + PLIC_REG_COMPLETE);
            trans.set_data_ptr((unsigned char*)&bid);
            socket->b_transport(trans, delay);
            wait(delay);

            // Now Kick System DMA
            std::cout << sc_time_stamp() << " CPU: Programming System DMA (0x4000 -> 0x5000, LEN=4)...\n";
            
            uint32_t src = 0x4000;
            uint32_t dst = 0x5000;
            uint32_t len = 4;
            uint32_t ctrl = 1;

            trans.set_command(tlm::TLM_WRITE_COMMAND);
            trans.set_address(DMA_BASE + DMA_REG_SRC);
            trans.set_data_ptr((unsigned char*)&src);
            socket->b_transport(trans, delay);
            wait(delay);

            trans.set_address(DMA_BASE + DMA_REG_DST);
            trans.set_data_ptr((unsigned char*)&dst);
            socket->b_transport(trans, delay);
            wait(delay);

            trans.set_address(DMA_BASE + DMA_REG_LEN);
            trans.set_data_ptr((unsigned char*)&len);
            socket->b_transport(trans, delay);
            wait(delay);

            trans.set_address(DMA_BASE + DMA_REG_CTRL);
            trans.set_data_ptr((unsigned char*)&ctrl);
            socket->b_transport(trans, delay);
            wait(delay);

        } else if (irq_id == IRQ_ID_DMA) {
            std::cout << sc_time_stamp() << " CPU: Handling System DMA Interrupt\n";
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

    std::cout << sc_time_stamp() << " CPU: All tasks finished with complex NVDLA pipeline.\n";
}

void CPU::check_response(tlm::tlm_generic_payload& trans)
{
    if (trans.get_response_status() != tlm::TLM_OK_RESPONSE) {
        std::cerr << sc_time_stamp() << " CPU TLM ERROR: " 
                  << trans.get_response_string() << "\n";
    }
}