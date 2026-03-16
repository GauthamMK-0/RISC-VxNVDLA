#include "nvdla.h"
#include <iostream>

void NVDLA::scheduler_thread()
{
    while (true) {
        wait(start_event | bdma_start_event);

        if (regs[NVDLA_REG_START >> 2] & 0x1) {
            regs[NVDLA_REG_STATUS >> 2] = 1; // Busy
            
            std::cout << sc_time_stamp() << " NVDLA: Scheduler starting compute pipeline\n";
            
            run_cdma();     // Step 1: Data Load
            run_compute();  // Step 2: Processing
            run_writeback();// Step 3: Result Store
            
            regs[NVDLA_REG_START >> 2] = 0;
            regs[NVDLA_REG_STATUS >> 2] = 0; // Idle
            
            // Interrupt
            irq.write(true);
            wait(10, SC_NS);
            irq.write(false);
        }

        if (regs[NVDLA_REG_BDMA_CTRL >> 2] & 0x1) {
            std::cout << sc_time_stamp() << " NVDLA: Scheduler starting BDMA transfer\n";
            run_bdma();
            regs[NVDLA_REG_BDMA_CTRL >> 2] = 0;
            
            irq.write(true);
            wait(10, SC_NS);
            irq.write(false);
        }
    }
}
