#ifndef IRQ_H
#define IRQ_H

#include <systemc>

using namespace sc_core;

struct IRQ : sc_module {

    sc_in<bool> nvdla_irq;
    sc_out<bool> cpu_irq;

    SC_CTOR(IRQ);

    void route();
};

#endif