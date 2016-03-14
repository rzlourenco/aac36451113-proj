#pragma once

#include "common.h"
#include <stddef.h>

struct cpu_state_t {
    address_t pc;

    int if_enable, id_enable, ex_enable, mem_enable, wb_enable;

    size_t total_cycles;
    size_t total_instructions;
};

struct msr_t {
    int c; /* carry flag */
    int i; /* immediate flag */
};

extern struct cpu_state_t cpu_state;
extern struct msr_t msr;

void init_cpu(void);
void clock(void);
