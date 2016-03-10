#include "cpu_state.h"

#include "ex_stage.h"
#include "id_stage.h"
#include "if_stage.h"
#include "mem_stage.h"
#include "wb_stage.h"

#include <string.h>

struct cpu_state cpu_state;

void init_cpu(void) {
    cpu_state.pc = make_address(0);
    cpu_state.halt = 0;

    cpu_state.if_enable = 1;
    cpu_state.id_enable = 1;
    cpu_state.ex_enable = 1;
    cpu_state.mem_enable = 1;
    cpu_state.wb_enable = 1;

    cpu_state.total_cycles = 0;
    cpu_state.total_instructions = 0;

    memset(&if_id_state, 0, sizeof(if_id_state));
    memset(&id_ex_state, 0, sizeof(id_ex_state));
    memset(&ex_mem_state, 0, sizeof(ex_mem_state));
    memset(&mem_wb_state, 0, sizeof(mem_wb_state));
}

void clock(void) {
    if (cpu_state.if_enable) {
        if_stage();
    }

    if (cpu_state.id_enable) {
        id_stage();
    }

    if (cpu_state.ex_enable) {
        ex_stage();
    }

    if (cpu_state.mem_enable) {
        mem_stage();
    }

    if (cpu_state.wb_enable) {
        wb_stage();
    }

    ++cpu_state.total_cycles;
}
