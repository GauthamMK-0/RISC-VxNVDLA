#include "nvdla.h"

NVDLA::NVDLA(sc_module_name name)
: sc_module(name),
  socket("socket"),
  dbb_socket("dbb_socket"),
  irq("irq")
{
    socket.register_b_transport(this, &NVDLA::b_transport);

    SC_THREAD(scheduler_thread);

    for (int i = 0; i < 64; i++) regs[i] = 0;
    irq.initialize(false);
}
