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
}

void cpu_update_pc(address_t pc) {

}

void cpu_dump(int signal) {

}
