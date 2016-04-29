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

struct cpu_t cpu_state;
void cpu_init(void) {
    cpu_state.halt = 0;

    bp_init();
}

int cpu_halt(void) {
    return cpu_state.halt;
}

void cpu_clock(void) {
    fprintf(stderr, "%s\n", __func__);

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

    cpu_state.halt = 1;
}

void cpu_update_pc(address_t pc) {

}

void cpu_dump(int signal) {

}
