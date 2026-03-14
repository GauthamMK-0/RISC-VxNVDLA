// ============================================================================
// top.cpp  –  TLM-2.0 SoC top-level netlist
//
// Topology:
//
//   CPU (initiator) ──┐
//                     ├──► AXI_IC ──► NVDLA  (0x40000000 – 0x4FFFFFFF)
//   DMA (initiator) ──┘         └──► MEMORY  (0x00000000 – 0x0FFFFFFF)
//
//   NVDLA.irq ──► IRQ controller ──► cpu_irq_sig (sc_signal)
//
// ============================================================================

#include <systemc>
#include "../CPU/cpu.h"
#include "../DMA/dma.h"
#include "../Memory/memory.h"
#include "../NVDLA/nvdla.h"
#include "../bus/axi_interconnect.h"
#include "../IRQ/irq.h"

using namespace sc_core;

int sc_main(int argc, char* argv[])
{
    // ── Signals ──────────────────────────────────────────────────────────────
    sc_signal<bool> nvdla_irq_sig("nvdla_irq_sig");
    sc_signal<bool> cpu_irq_sig  ("cpu_irq_sig");

    // ── Module instantiation ─────────────────────────────────────────────────
    CPU     cpu    ("CPU");
    DMA     dma    ("DMA");
    AXI_IC  ic     ("AXI_IC");
    NVDLA   nvdla  ("NVDLA");
    MEMORY  memory ("MEMORY");
    IRQ     irq_ctrl("IRQ");

    // ── TLM socket bindings ──────────────────────────────────────────────────
    // Masters → Interconnect slave ports
    cpu.socket      .bind(ic.cpu_socket);
    dma.mem_socket  .bind(ic.dma_socket);

    // Interconnect master ports → Slaves
    ic.nvdla_socket .bind(nvdla.socket);
    ic.mem_socket   .bind(memory.socket);

    // ── Interrupt signal bindings ────────────────────────────────────────────
    nvdla.irq       (nvdla_irq_sig);
    irq_ctrl.nvdla_irq(nvdla_irq_sig);
    irq_ctrl.cpu_irq  (cpu_irq_sig);

    // ── Run simulation long enough to capture all events ─────────────────────
    // Timeline: CPU@10ns, DMA@~200ns, NVDLA-finish@~550ns + 10ns IRQ pulse
    sc_start(700, SC_NS);

    std::cout << "\n[sim] Simulation complete at "
              << sc_time_stamp() << std::endl;

    return 0;
}