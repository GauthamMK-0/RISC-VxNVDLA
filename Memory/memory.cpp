#include "memory.h"
#include <cstring>

MEMORY::MEMORY(sc_module_name name)
: sc_module(name), socket("socket")
{
    socket.register_b_transport(this, &MEMORY::b_transport);
}

void MEMORY::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
{
    auto cmd = trans.get_command();
    auto addr = trans.get_address();
    auto ptr = trans.get_data_ptr();
    auto len = trans.get_data_length();

    delay += sc_time(50, SC_NS);

    if (cmd == tlm::TLM_WRITE_COMMAND)
        memcpy(&mem[addr], ptr, len);
    else
        memcpy(ptr, &mem[addr], len);

    trans.set_response_status(tlm::TLM_OK_RESPONSE);
}