#include "mem_stage.h"

#include "cpu_state.h"
#include "ex_stage.h"
#include "memory.h"
#include "wb_stage.h"

struct mem_state_t mem_state;

void mem_stage(void) {
    // These pass unchanged to the next stage
    wb_state.pc = mem_state.pc;
    wb_state.select_data = mem_state.wb_select_data;
    wb_state.write_enabled = mem_state.wb_write_enable;
    wb_state.dest_address = mem_state.wb_dest_register;
    wb_state.alu_result = mem_state.alu_result;

    if (mem_state.memory_access) {
        wb_state.memory_out = memory_read(mem_state.alu_result);
        if (mem_state.write_enable) {
            memory_write(mem_state.alu_result, mem_state.data);
        }
    }
}
