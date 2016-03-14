#pragma once

#include "common.h"

struct ex_mem_state_t {
    address_t next_pc;
    int cond;
    word_t a;
    word_t s;
    int mem_write;
    int sel_out;
    int write_enabled;
    int dest_address;
};

enum alu_ops {
    ALU_ADD  = 0,
    ALU_SUB  = 1,
    ALU_ADDC = 2,
    ALU_SUBC = 3,
    ALU_MUL  = 4,
};

extern struct ex_mem_state_t ex_mem_state;

void ex_stage(void);
