#pragma once

#include "common.h"
#include <stddef.h>

struct cpu_state_t {
    address_t pc;

    int halt;
    int if_enable, id_enable, ex_enable, mem_enable, wb_enable;

    size_t total_cycles;
    size_t total_instructions;
};

struct msr_t {
    int32_t _unused0 : 29;
    int32_t c : 1; /* carry flag */
    int32_t _unused1 : 2;
};

extern struct cpu_state_t cpu_state;
extern struct msr_t msr;

void init_cpu(void);
int cpu_halt(void);
void clock(void);
