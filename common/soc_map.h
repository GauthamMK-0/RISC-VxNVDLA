#ifndef SOC_MAP_H
#define SOC_MAP_H

#include <systemc>

// Base Addresses
const uint64_t DRAM_BASE    = 0x00000000;
const uint64_t DRAM_SIZE    = 0x10000000; // 256 MB

const uint64_t NVDLA_BASE   = 0x40000000;
const uint64_t NVDLA_SIZE   = 0x00100000; // 1 MB

const uint64_t DMA_BASE     = 0x50000000;
const uint64_t DMA_SIZE     = 0x00100000; // 1 MB

const uint64_t PLIC_BASE    = 0x60000000;
const uint64_t PLIC_SIZE    = 0x00100000; // 1 MB

// Register Offsets
// DMA Registers
const uint32_t DMA_REG_SRC      = 0x00;
const uint32_t DMA_REG_DST      = 0x04;
const uint32_t DMA_REG_LEN      = 0x08;
const uint32_t DMA_REG_CTRL     = 0x0C; // Bit 0: Start

// NVDLA Registers
const uint32_t NVDLA_REG_START  = 0x00;
const uint32_t NVDLA_REG_STATUS = 0x04; // 0: Idle, 1: Busy

// PLIC Registers
const uint32_t PLIC_REG_SOURCE    = 0x00; // Pending bitmask
const uint32_t PLIC_REG_ENABLE    = 0x04; // Enable bitmask
const uint32_t PLIC_REG_CLAIM     = 0x08; // Read to get current highest priority IRQ ID
const uint32_t PLIC_REG_COMPLETE  = 0x0C; // Write to clear/complete IRQ

// Interrupt IDs
const uint32_t IRQ_ID_NVDLA = 1;
const uint32_t IRQ_ID_DMA   = 2;

#endif
