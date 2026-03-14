#ifndef NVDLA_H
#define NVDLA_H

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

SC_MODULE(NVDLA) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_in<bool> start;
    sc_out<bool> busy;
    sc_out<bool> irq;

    unsigned int compute_latency;

    SC_CTOR(NVDLA);
    void compute_process();
};

#endif