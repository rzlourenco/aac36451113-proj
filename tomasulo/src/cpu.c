#include "cpu.h"

#include "branch_predictor.h"
#include "cdb.h"
#include "commit.h"
#include "execute.h"
#include "fetch.h"
#include "issue.h"
#include "memory.h"
#include "register.h"
#include "rob.h"

struct cpu_t cpu_state;

address_t new_pc;
int has_new_pc;

void cpu_init(void) {
    bp_init();
}

int cpu_halt(void) {
    return 1;
}

void cpu_clock(void) {
    commit_clock();
    execute_clock();
    issue_clock();
    fetch_clock();

    bp_clock();
    memory_clock();
    register_clock();

    rob_clock();
    cdb_clock();

    if (has_new_pc) {
        cpu_state.pc = new_pc;
        has_new_pc = 0;
    }
}

void cpu_update_pc(address_t pc) {
    has_new_pc = 1;
    new_pc = pc;
}

void cpu_dump(int signal) {

}
