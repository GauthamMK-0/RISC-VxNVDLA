#ifndef CPU_H
#define CPU_H

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

SC_MODULE(CPU) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_out<bool> start_dma;
    sc_out<unsigned int> dma_addr;
    sc_out<bool> start_nvdla;

    SC_CTOR(CPU);
    void generate_traffic();
};

#endif