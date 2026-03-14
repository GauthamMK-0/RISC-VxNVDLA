#include "memory.h"
#include <iostream>

DRAM::DRAM(sc_module_name name) : sc_module(name) {
    memory_latency = 10;
    SC_THREAD(process_request);
    sensitive << clk.pos();
    async_reset_signal_is(rst, true);
}

void DRAM::process_request() {
    ready.write(false);
    while (true) {
        wait();
        if (req.read() == true) {
            ready.write(false);
            wait(memory_latency, SC_NS);
            ready.write(true);
        }
    }
}