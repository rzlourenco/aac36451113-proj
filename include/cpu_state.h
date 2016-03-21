#pragma once

#include "common.h"
#include <stddef.h>

struct cpu_state_t {
    size_t total_cycles;
    size_t total_instructions;

    address_t pc;

    union {
        struct {
            word_t if_enable : 1;
            word_t id_enable : 1;
            word_t ex_enable : 1;
            word_t mem_enable : 1;
            word_t wb_enable : 1;
        };
        word_t pipeline_stages;
    };

    int halt;
    int branching;
    int delayed;
    int if_stalls;
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
void cpu_dump(int signal);
