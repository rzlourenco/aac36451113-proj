#include "cpu_state.h"

#include "ex_stage.h"
#include "id_stage.h"
#include "if_stage.h"
#include "mem_stage.h"
#include "wb_stage.h"
#include "register.h"

struct cpu_state_t cpu_state;
struct msr_t msr;
h_word_t rIMM;
int little_endian;

void init_cpu(void) {
    cpu_state = (struct cpu_state_t){ 0 };

    cpu_state.pc = 0;

    cpu_state.halt = 0;
    cpu_state.has_delayed_branch = 0;

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

    if_state = (struct if_state_t){ 0 };
    id_state = (struct id_state_t){ 0 };
    ex_state = (struct ex_state_t){ 0 };
    mem_state = (struct mem_state_t){ 0 };
    wb_state = (struct wb_state_t){ 0 };
}

int cpu_halt(void) {
    return cpu_state.halt || (!cpu_state.if_enable && !cpu_state.id_enable && !cpu_state.ex_enable &&
           !cpu_state.mem_enable && !cpu_state.wb_enable);
}

void clock(void) {
    // Advance register-in-use state
    register_clock();

    if (cpu_state.if_stalls > 0) {
        cpu_state.if_stalls -= 1;
    }

    if (cpu_state.wb_enable) {
        cpu_state.wb_enable = 0;

        wb_stage();

        wb_state = (struct wb_state_t){ 0 };
    }

    if (cpu_state.mem_enable) {
        cpu_state.wb_enable = 1;
        cpu_state.mem_enable = 0;

        mem_stage();

        mem_state = (struct mem_state_t){ 0 };
    }

    if (cpu_state.ex_enable) {
        cpu_state.mem_enable = 1;
        cpu_state.ex_enable = 0;

        ex_stage();

        ex_state = (struct ex_state_t){ 0 };
    }

    if (cpu_state.id_enable) {
        cpu_state.ex_enable = 1;
        cpu_state.id_enable = 0;
        cpu_state.id_stall = 0;

        id_stage();

        id_state = (struct id_state_t){ 0 };
    }

    if (cpu_state.if_enable) {
        cpu_state.id_enable = 1;

        if_stage();
    }

    ++cpu_state.total_cycles;
}

void cpu_dump(int signal) {
    fprintf(stderr, "\n");

    fprintf(stderr, "\tAfter %zu clock cycle(s)\n", cpu_state.total_cycles);
    fprintf(stderr,
            "\tpc=%08x msr{ .c=%d .i=%d } rIMM=%04x if_stalls=%d has_delayed_branch=%d\n"
            "",
            cpu_state.pc,
            msr.c,
            msr.i,
            rIMM,
            cpu_state.if_stalls,
            cpu_state.has_delayed_branch);

    fprintf(stderr, "\tif(%d) next_pc=%08x branch_pc=%08x pc_sel=%d\n",
            cpu_state.if_enable,
            if_state.next_pc,
            if_state.branch_pc,
            if_state.pc_sel);

    fprintf(stderr, "\tid(%d) id_stall=%d pc=%08x instruction=%08x\n",
            cpu_state.id_enable,
            cpu_state.id_stall,
            id_state.pc,
            id_state.instruction);

    fprintf(stderr, "\tex(%d) pc=%08x branch_enable=%d\n",
            cpu_state.ex_enable,
            ex_state.pc,
            ex_state.branch_enable);

    fprintf(stderr, "\tmem(%d) pc=%08x write_enable=%d\n",
            cpu_state.mem_enable,
            mem_state.pc,
            mem_state.write_enable);

    fprintf(stderr, "\twb(%d) pc=%08x write_enable=%d\n",
            cpu_state.wb_enable,
            wb_state.pc,
            wb_state.write_enable);

    fprintf(stderr, "\n");
    register_dump();
    fprintf(stderr, "\n");

    if (signal) {
        exit(signal);
    }

    fflush(stderr);
}
