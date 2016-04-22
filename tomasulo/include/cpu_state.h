#pragma once

#include "common.h"

#include <stddef.h>

struct cpu_state_t {
    uint64_t total_cycles;
    uint64_t total_instructions;

    address_t pc;
};

struct msr_t {
    word_t c : 1;
    word_t i : 1;
};

extern struct cpu_state_t cpu_state;

void init_cpu(void);
int cpu_halt(void);
void clock(void);
void cpu_dump(int signal);
