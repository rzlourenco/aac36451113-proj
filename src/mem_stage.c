#include "mem_stage.h"

#include "cpu_state.h"
#include "ex_stage.h"
#include "memory.h"

struct mem_wb_state_t mem_wb_state;

void mem_stage(void) {
    // These pass unchanged to the next stage
    mem_wb_state.pc = ex_mem_state.pc;
    mem_wb_state.alu_result = ex_mem_state.alu_result;
    mem_wb_state.sel_out = ex_mem_state.sel_out;
    mem_wb_state.write_enabled = ex_mem_state.write_enabled;
    mem_wb_state.dest_address = ex_mem_state.dest_address;

    mem_wb_state.memory_out = memory_read(ex_mem_state.alu_result);
    if (ex_mem_state.mem_write) {
        memory_write(ex_mem_state.alu_result, ex_mem_state.op_a);
    }

    cpu_state.mem_enable = cpu_state.ex_enable;
    cpu_state.wb_enable = 1;
}
