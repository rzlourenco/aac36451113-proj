#pragma once

#include "common.h"

struct id_ex_state_t {
    address_t pc;

    word_t op_a, op_b, op_c, imm;

    int select_operation, select_op_a, select_op_b;

    int cond;
    int mem_write;
    int sel_out;
    int dest_address;
    int write_enabled;

    int keep_carry;
    int is_mem;
};

extern struct id_ex_state_t id_ex_state;

void id_stage(void);
