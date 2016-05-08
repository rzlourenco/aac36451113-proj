#pragma once

#include "common.h"

#include <stddef.h>

struct cpu_state_t {
    size_t total_cycles;
    size_t total_instructions;
    size_t branches_predicted;
    size_t branches;

    address_t pc;

    int halt;
    int has_delayed_branch;

    int if_stalls;
    int id_stall;

    word_t if_enable;
    word_t id_enable;
    word_t ex_enable;
    word_t mem_enable;
    word_t wb_enable;
};

struct msr_t {
    word_t c : 1;
    word_t i : 1;
};

extern struct cpu_state_t cpu_state;
extern struct msr_t msr;
extern uint16_t rIMM;

extern int little_endian;

extern int debug;

extern int has_breakpoint;
extern address_t breakpoint;

extern int has_mem_trace;
extern address_t mem_trace;

extern address_t trace_register;

extern FILE *trace_functions;

void init_cpu(void);
int cpu_halt(void);
void clock(void);
void cpu_dump(int signal);
