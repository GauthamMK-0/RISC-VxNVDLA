#include "axi_interconnect.h"

AXI_IC::AXI_IC(sc_module_name name)
: sc_module(name),
  cpu_socket("cpu_socket"),
  dma_socket("dma_socket"),
  mem_socket("mem_socket"),
  nvdla_socket("nvdla_socket"),
  dma_ctrl_socket("dma_ctrl_socket")
{
    cpu_socket.register_b_transport(this, &AXI_IC::b_transport);
    dma_socket.register_b_transport(this, &AXI_IC::b_transport);
}

void AXI_IC::b_transport(int id,
                         tlm::tlm_generic_payload& trans,
                         sc_time& delay)
{
    auto addr = trans.get_address();

    // Address map:
    //   0x40000000 – 0x4FFFFFFF  →  NVDLA registers
    //   0x50000000 – 0x5FFFFFFF  →  DMA registers
    //   everything else          →  DRAM
    if (addr >= 0x40000000 && addr < 0x50000000)
    {
        trans.set_address(addr - 0x40000000);
        nvdla_socket->b_transport(trans, delay);
    }
    else if (addr >= 0x50000000 && addr < 0x60000000)
    {
        trans.set_address(addr - 0x50000000);
        dma_ctrl_socket->b_transport(trans, delay);
    }
    else
    {
        mem_socket->b_transport(trans, delay);
    }
}