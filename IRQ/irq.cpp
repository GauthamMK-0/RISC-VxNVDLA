#include "irq.h"

IRQ::IRQ(sc_module_name name)
: sc_module(name)
{
    SC_METHOD(route);
    sensitive << nvdla_irq << dma_irq;
}

void IRQ::route()
{
    cpu_irq.write(nvdla_irq.read() || dma_irq.read());
}