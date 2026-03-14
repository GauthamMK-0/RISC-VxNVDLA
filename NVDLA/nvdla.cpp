#include "nvdla.h"

NVDLA::NVDLA(sc_module_name name) : sc_module(name) {
    compute_latency = 20;
    SC_THREAD(compute_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst, true);
}

void NVDLA::compute_process() {
    busy.write(false);
    irq.write(false);

    while (true) {
        wait();
        if (start.read() == true) {
            busy.write(true);
            irq.write(false);

            wait(compute_latency, SC_NS);

            busy.write(false);
            irq.write(true);
        } else {
            irq.write(false);
        }
    }
}