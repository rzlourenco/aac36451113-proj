#pragma once

#include "common.h"

struct ex_mem_state_t {
    address_t pc;
    int cond;
    word_t op_a;
    word_t alu_result;
    int mem_write;
    int sel_out;
    int write_enabled;
    int dest_address;

    int is_mem;
};

enum {
    ALU_ADD,
    ALU_SUB,
    ALU_CMP,
};

extern struct ex_mem_state_t ex_mem_state;

void ex_stage(void);
