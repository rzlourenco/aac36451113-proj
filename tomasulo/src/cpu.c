#include <cpu.h>
#include "cpu.h"

#include "branch_predictor.h"
#include "cdb.h"
#include "commit.h"
#include "dispatch.h"
#include "execute.h"
#include "fetch.h"
#include "issue.h"
#include "memory.h"
#include "register.h"
#include "rob.h"

struct cpu_stats cpu_stats;

enum {
    PC_SEL_FETCH,
    PC_SEL_BRANCH,
};

static struct internal_cpu_state {
    int halt;

    int pc_sel;
    address_t w_pc_fetch;
    address_t w_pc_branch;
    address_t r_pc;
} cpu_state;

void cpu_init(void) {
    cpu_state.halt = 0;
    cpu_state.r_pc = 0;
    cpu_state.pc_sel = PC_SEL_FETCH;

    bp_init();
}

void cpu_halt(void) {
    cpu_state.halt = 1;
}

int cpu_has_halted(void) {
    return cpu_state.halt;
}

void update_state(void) {
    switch (cpu_state.pc_sel) {
        case PC_SEL_FETCH:
            cpu_state.r_pc = cpu_state.w_pc_fetch;
            break;
        case PC_SEL_BRANCH:
            cpu_state.r_pc = cpu_state.w_pc_branch;
            break;
        default:
            ABORT_WITH_MSG("unknown pc_sel");
    }

    cpu_state.pc_sel = PC_SEL_FETCH;
}

void cpu_clock(void) {
    update_state();

    fetch_clock();
    issue_clock();
    dispatch_clock();
    execute_clock();
    commit_clock();

    bp_clock();
    memory_clock();
    register_clock();

    rob_clock();
    cdb_clock();

    cpu_stats.cycles += 1;
}

address_t cpu_get_pc(void) {
    return cpu_state.r_pc;
}

void cpu_update_pc(address_t pc) {
    cpu_state.w_pc_fetch = pc;
}

void cpu_branch(address_t pc) {
    cpu_state.w_pc_branch = pc;
    cpu_state.pc_sel = PC_SEL_BRANCH;
}

void cpu_dump(void) {
    if (!debug)
        return;

    fprintf(stderr, "pc:%08x\n", cpu_state.r_pc);
}
