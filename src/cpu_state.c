#include "cpu_state.h"

#include "ex_stage.h"
#include "id_stage.h"
#include "if_stage.h"
#include "mem_stage.h"
#include "wb_stage.h"

#include <string.h>

struct cpu_state_t cpu_state;
struct msr_t msr;
uint16_t rIMM;

void init_cpu(void) {
    cpu_state.pc = 0;

    cpu_state.halt = 0;

    cpu_state.if_enable = 1;
    cpu_state.id_enable = 0;
    cpu_state.ex_enable = 0;
    cpu_state.mem_enable = 0;
    cpu_state.wb_enable = 0;

    cpu_state.total_cycles = 0;
    cpu_state.total_instructions = 0;

    msr.c = 0;
    msr.i = 0;

    rIMM = 0;

    memset(&if_state, 0, sizeof(if_state));
    memset(&id_state, 0, sizeof(id_state));
    memset(&ex_state, 0, sizeof(ex_state));
    memset(&mem_state, 0, sizeof(mem_state));
    memset(&wb_state, 0, sizeof(wb_state));
}

int cpu_halt(void) {
    return cpu_state.halt && !cpu_state.if_enable && !cpu_state.id_enable && !cpu_state.ex_enable &&
           !cpu_state.mem_enable && !cpu_state.wb_enable;
}

void clock(void) {
    if (cpu_state.wb_enable)
        wb_stage();

    if (cpu_state.mem_enable)
        mem_stage();

    if (cpu_state.ex_enable)
        ex_stage();

    if (cpu_state.id_enable)
        id_stage();

    if (cpu_state.if_enable)
        if_stage();

    ++cpu_state.total_cycles;
}
