#include "cpu.h"
#include <iostream>

CPU::CPU(sc_module_name name) : sc_module(name) {
    SC_THREAD(generate_traffic);
    sensitive << clk.pos();
    async_reset_signal_is(rst, true);
}

void CPU::generate_traffic() {
    start_dma.write(false);
    start_nvdla.write(false);

    wait(); // wait for reset

    while (true) {
        wait();

        start_dma.write(true);
        dma_addr.write(0x1000);
        wait();
        start_dma.write(false);

        wait(5, SC_NS);

        start_nvdla.write(true);
        wait();
        start_nvdla.write(false);

        wait(50, SC_NS);
    }
}