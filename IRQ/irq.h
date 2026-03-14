#ifndef IRQ_H
#define IRQ_H

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

SC_MODULE(IRQ_CTRL) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_in<bool> nvdla_irq;
    sc_in<bool> dma_done;
    sc_out<bool> cpu_irq;

    SC_CTOR(IRQ_CTRL);
    void irq_process();
};

#endif