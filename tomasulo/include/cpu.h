#pragma once

#include "common.h"

#include <stddef.h>

struct cpu_stats {
    uint64_t cycles;
    uint64_t instructions;

    uint64_t sc_issue;
    uint64_t sc_dispatch;
    uint64_t sc_execute;
    uint64_t sc_execute_alu;
    uint64_t sc_commit;
    uint64_t sc_commit_empty;
    uint64_t sc_commit_stall;

    uint64_t sc_cdb;
    uint64_t sc_rob;
};

extern struct cpu_stats cpu_stats;

void cpu_init(void);
void cpu_clock(void);
void cpu_dump(void);
void cpu_halt(void);
int cpu_has_halted(void);

address_t cpu_get_pc(void);
void cpu_update_pc(address_t pc);
void cpu_branch(address_t pc);
