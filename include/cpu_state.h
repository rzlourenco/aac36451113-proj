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
    word_t c : 1;
    word_t i : 1;
};

extern struct cpu_state_t cpu_state;
extern struct msr_t msr;
extern uint16_t rIMM;

void init_cpu(void);
int cpu_halt(void);
void clock(void);
