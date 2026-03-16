#include "axi_interconnect.h"
#include "../common/soc_map.h"

AXI_IC::AXI_IC(sc_module_name name)
: sc_module(name),
  cpu_socket("cpu_socket"),
  dma_socket("dma_socket"),
  nvdla_dbb_socket("nvdla_dbb_socket"),
  mem_socket("mem_socket"),
  nvdla_socket("nvdla_socket"),
  dma_ctrl_socket("dma_ctrl_socket"),
  plic_socket("plic_socket")
{
    cpu_socket.register_b_transport(this, &AXI_IC::b_transport);
    dma_socket.register_b_transport(this, &AXI_IC::b_transport);
    nvdla_dbb_socket.register_b_transport(this, &AXI_IC::b_transport);
}

void AXI_IC::b_transport(int id,
                         tlm::tlm_generic_payload& trans,
                         sc_time& delay)
{
    auto addr = trans.get_address();

    // Address map routing using soc_map.h
    if (addr >= NVDLA_BASE && addr < (NVDLA_BASE + NVDLA_SIZE))
    {
        trans.set_address(addr - NVDLA_BASE);
        nvdla_socket->b_transport(trans, delay);
    }
    else if (addr >= DMA_BASE && addr < (DMA_BASE + DMA_SIZE))
    {
        trans.set_address(addr - DMA_BASE);
        dma_ctrl_socket->b_transport(trans, delay);
    }
    else if (addr >= PLIC_BASE && addr < (PLIC_BASE + PLIC_SIZE))
    {
        trans.set_address(addr - PLIC_BASE);
        plic_socket->b_transport(trans, delay);
    }
    else if (addr >= DRAM_BASE && addr < (DRAM_BASE + DRAM_SIZE))
    {
        trans.set_address(addr - DRAM_BASE);
        mem_socket->b_transport(trans, delay);
    }
    else
    {
        // Default error response for unmapped regions
        trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
    }
}