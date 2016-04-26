#pragma once

#include "common.h"

#include <stddef.h>

struct cpu_t {
    uint64_t total_cycles;
    uint64_t total_instructions;

    address_t pc;
};

struct msr_t {
    word_t c : 1;
    word_t i : 1;
};

extern struct cpu_t cpu_state;

void cpu_clock(void);
void cpu_dump(int signal);
int cpu_halt(void);
void cpu_init(void);
void cpu_update_pc(address_t pc);
