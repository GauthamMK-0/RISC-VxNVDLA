#ifndef SOC_MAP_H
#define SOC_MAP_H

#include <systemc>

// Memory Map Constants
const uint64_t DRAM_BASE    = 0x00000000;
const uint64_t DRAM_SIZE    = 0x10000000; // 256 MB

const uint64_t NVDLA_BASE   = 0x40000000;
const uint64_t NVDLA_SIZE   = 0x00100000; // 1 MB

const uint64_t DMA_BASE     = 0x50000000;
const uint64_t DMA_SIZE     = 0x00100000; // 1 MB

#endif
