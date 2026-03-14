#include <systemc>
#include "../Memory/memory.h"
#include "../CPU/cpu.h"
#include "../NVDLA/nvdla.h"
#include "../DMA/dma.h"
#include "../IRQ/irq.h"

int sc_main(int argc, char* argv[]) {
    sc_signal<bool> clk, rst;
    sc_signal<bool> cpu_start_dma, cpu_start_nvdla;
    sc_signal<unsigned int> dma_addr;
    sc_signal<bool> nvdla_busy, nvdla_irq;
    sc_signal<bool> dma_done;
    sc_signal<bool> cpu_irq;
    sc_signal<bool> mem_req;
    sc_signal<unsigned int> mem_addr;
    sc_signal<bool> mem_ready;

    // Instantiate modules
    CPU cpu("CPU");
    cpu.clk(clk); cpu.rst(rst);
    cpu.start_dma(cpu_start_dma);
    cpu.dma_addr(dma_addr);
    cpu.start_nvdla(cpu_start_nvdla);

    DMA dma("DMA");
    dma.clk(clk); dma.rst(rst);
    dma.start(cpu_start_dma); dma.addr(dma_addr);
    dma.done(dma_done);

    NVDLA nvdla("NVDLA");
    nvdla.clk(clk); nvdla.rst(rst);
    nvdla.start(cpu_start_nvdla);
    nvdla.busy(nvdla_busy);
    nvdla.irq(nvdla_irq);

    IRQ_CTRL irq("IRQ");
    irq.clk(clk); irq.rst(rst);
    irq.nvdla_irq(nvdla_irq);
    irq.dma_done(dma_done);
    irq.cpu_irq(cpu_irq);

    DRAM mem("MEM");
    mem.clk(clk); mem.rst(rst);
    mem.req(mem_req); mem.addr(mem_addr); mem.ready(mem_ready);

    // Simulation clock
    sc_time period(10, SC_NS);
    sc_start(0, SC_NS);
    rst = true;
    clk = false;
    sc_start(period);
    rst = false;

    for (int i = 0; i < 50; i++) {
        clk = !clk;
        sc_start(period);
    }

    return 0;
}