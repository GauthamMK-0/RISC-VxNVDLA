#ifndef MEMORY_H
#define MEMORY_H

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

SC_MODULE(DRAM) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_in<bool> req;
    sc_in<unsigned int> addr;
    sc_out<bool> ready;

    unsigned int memory_latency;

    SC_CTOR(DRAM);
    void process_request();
};

#endif