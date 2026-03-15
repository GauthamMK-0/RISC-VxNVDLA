// ============================================================================
// top.cpp  –  TLM-2.0 SoC top-level netlist
//
// Topology:
//
//   CPU (initiator) ──┐
//                     ├──► AXI_IC ──┬──► NVDLA  (0x40000000)
//   DMA (initiator) ──┘             ├──► DMA    (0x50000000)
//                                   ├──► PLIC   (0x60000000)
//                                   └──► MEMORY (0x00000000)
//
//   NVDLA.irq ──┐
//               ├──► PLIC ──► cpu_irq_sig ──► CPU.irq_in
//   DMA.irq_out ┘
//
// ============================================================================

#include <systemc>
#include "../CPU/cpu.h"
#include "../DMA/dma.h"
#include "../Memory/memory.h"
#include "../NVDLA/nvdla.h"
#include "../bus/axi_interconnect.h"
#include "../IRQ/plic.h"

using namespace sc_core;

int sc_main(int argc, char* argv[])
{
    // ── Signals ──────────────────────────────────────────────────────────────
    sc_signal<bool> nvdla_irq_sig("nvdla_irq_sig");
    sc_signal<bool> dma_irq_sig  ("dma_irq_sig");
    sc_signal<bool> cpu_irq_sig  ("cpu_irq_sig");

    // ── Module instantiation ─────────────────────────────────────────────────
    CPU     cpu    ("CPU");
    DMA     dma    ("DMA");
    AXI_IC  ic     ("AXI_IC");
    NVDLA   nvdla  ("NVDLA");
    MEMORY  memory ("MEMORY");
    PLIC    plic   ("PLIC");

    // ── TLM socket bindings ──────────────────────────────────────────────────
    // Masters → Interconnect slave ports
    cpu.socket      .bind(ic.cpu_socket);
    dma.mem_socket  .bind(ic.dma_socket);

    // Interconnect master ports → Slaves
    ic.nvdla_socket   .bind(nvdla.socket);
    ic.mem_socket     .bind(memory.socket);
    ic.dma_ctrl_socket.bind(dma.target_socket);
    ic.plic_socket    .bind(plic.socket);

    // ── Interrupt signal bindings ────────────────────────────────────────────
    nvdla.irq       (nvdla_irq_sig);
    dma.irq_out     (dma_irq_sig);
    
    plic.nvdla_irq  (nvdla_irq_sig);
    plic.dma_irq    (dma_irq_sig);
    plic.cpu_irq    (cpu_irq_sig);
    
    cpu.irq_in      (cpu_irq_sig);

    // ── Run simulation ───────────────────────────────────────────────────────
    sc_start(2000, SC_NS);

    std::cout << "\n[sim] Simulation complete at "
              << sc_time_stamp() << std::endl;

    return 0;
}