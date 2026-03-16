#include "nvdla.h"
#include <iostream>

void NVDLA::run_writeback()
{
    uint32_t dst = regs[NVDLA_REG_WR_DST >> 2];
    std::cout << sc_time_stamp() << " NVDLA [Writeback]: Storing results to 0x" << std::hex << dst << std::dec << "...\n";
    
    uint32_t result = 0x55AA55AA;
    mem_access(true, dst, (unsigned char*)&result, 4);
    
    wait(100, SC_NS); 
}

void NVDLA::run_bdma()
{
    uint32_t src = regs[NVDLA_REG_BDMA_SRC >> 2];
    uint32_t dst = regs[NVDLA_REG_BDMA_DST >> 2];
    uint32_t len = regs[NVDLA_REG_BDMA_LEN >> 2];
    
    std::cout << sc_time_stamp() << " NVDLA [BDMA]: Transferring 0x" << std::hex << src 
              << " -> 0x" << dst << " (len " << std::dec << len << ")\n";
    
    uint32_t buf;
    mem_access(false, src, (unsigned char*)&buf, 4);
    wait(50, SC_NS);
    mem_access(true, dst, (unsigned char*)&buf, 4);
    wait(50, SC_NS);
}
