#include "nvdla.h"
#include <iostream>

void NVDLA::run_cdma()
{
    uint32_t src = regs[NVDLA_REG_CDMA_SRC >> 2];
    std::cout << sc_time_stamp() << " NVDLA [CDMA]: Fetching data from 0x" << std::hex << src << std::dec << "...\n";
    
    uint32_t dummy_buf;
    mem_access(false, src, (unsigned char*)&dummy_buf, 4);
    
    wait(100, SC_NS); // Simulated Latency
}
