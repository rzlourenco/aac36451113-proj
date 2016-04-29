#pragma once

#include "common.h"

#include <stddef.h>

struct cpu_t {
    struct {
        uint64_t cycles;
        uint64_t instructions;

        uint64_t cdb_stalls;
        uint64_t issue_stalls;
    } stats;

    int halt;

    address_t pc;
};

extern struct cpu_t cpu_state;

void cpu_clock(void);
void cpu_dump(int signal);
int cpu_halt(void);
void cpu_init(void);
void cpu_update_pc(address_t pc);
