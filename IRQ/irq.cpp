#include "irq.h"

IRQ_CTRL::IRQ_CTRL(sc_module_name name) : sc_module(name) {
    SC_THREAD(irq_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst, true);
}

void IRQ_CTRL::irq_process() {
    cpu_irq.write(false);

    while (true) {
        wait();
        if (nvdla_irq.read() || dma_done.read()) {
            cpu_irq.write(true);
        } else {
            cpu_irq.write(false);
        }
    }
}