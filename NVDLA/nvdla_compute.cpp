#include "nvdla.h"
#include <iostream>

void NVDLA::run_compute()
{
    std::cout << sc_time_stamp() << " NVDLA [Compute]: Executing modular core (SDP/PDP/CDP)...\n";
    
    // Pipeline delays
    wait(200, SC_NS); // Conv
    wait(100, SC_NS); // SDP
    wait(50, SC_NS);  // PDP
}
