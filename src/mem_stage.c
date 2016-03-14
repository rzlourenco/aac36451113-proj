#include "ex_stage.h"
#include "mem_stage.h"
#include "memory.h"

struct mem_wb_state_t mem_wb_state;

void mem_stage(void) {
    word_t opa, opb;

    // These pass unchanged to the next stage
    mem_wb_state.next_pc = ex_mem_state.next_pc;
    mem_wb_state.s = ex_mem_state.s;
    mem_wb_state.sel_out = ex_mem_state.sel_out;
    mem_wb_state.write_enabled = ex_mem_state.write_enabled;
    mem_wb_state.dest_address = ex_mem_state.dest_address;

    if (!ex_mem_state.mem_write) {
        mem_wb_state.data_out = memory_read(ex_mem_state.s);
    } else {
        memory_write(ex_mem_state.s,
                     ex_mem_state.a,
                     ex_mem_state.mem_write);
    }
}
