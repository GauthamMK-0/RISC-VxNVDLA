#include "dma.h"

DMA::DMA(sc_module_name name) : sc_module(name) {
    burst_latency = 5;
    SC_THREAD(dma_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst, true);
}

void DMA::dma_process() {
    done.write(false);

    while (true) {
        wait();
        if (start.read() == true) {
            done.write(false);
            wait(burst_latency, SC_NS);
            done.write(true);
        }
    }
}