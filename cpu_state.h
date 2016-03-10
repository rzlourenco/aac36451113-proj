#pragma once

#include "common.h"
#include <stddef.h>s

struct cpu_state {
    address_t pc;
    int halt;

    int if_enable, id_enable, ex_enable, mem_enable, wb_enable;

    size_t total_cycles;
    size_t total_instructions;
};

extern struct cpu_state cpu_state;

void init_cpu(void);
void clock(void);
