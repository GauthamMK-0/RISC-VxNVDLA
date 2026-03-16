#include "nvdla.h"
#include <iostream>

void NVDLA::mem_access(bool write, uint64_t addr, unsigned char* data, unsigned int len)
{
    tlm::tlm_generic_payload trans;
    sc_time delay = SC_ZERO_TIME;

    trans.set_command(write ? tlm::TLM_WRITE_COMMAND : tlm::TLM_READ_COMMAND);
    trans.set_address(addr);
    trans.set_data_ptr(data);
    trans.set_data_length(len);
    trans.set_streaming_width(len);
    trans.set_byte_enable_ptr(nullptr);
    trans.set_dmi_allowed(false);
    trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

    dbb_socket->b_transport(trans, delay);
    wait(delay);

    if (trans.get_response_status() != tlm::TLM_OK_RESPONSE) {
        std::cerr << sc_time_stamp() << " NVDLA DBB ERROR: " << trans.get_response_string() << "\n";
    }
}
