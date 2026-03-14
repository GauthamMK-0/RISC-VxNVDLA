#ifndef DMA_H
#define DMA_H

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

SC_MODULE(DMA) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_in<bool> start;
    sc_in<unsigned int> addr;
    sc_out<bool> done;

    unsigned int burst_latency;

    SC_CTOR(DMA);
    void dma_process();
};

#endif