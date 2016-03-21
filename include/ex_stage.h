#pragma once

#include "common.h"

struct ex_state_t {
    address_t pc;

    word_t op_a, op_b, op_c;
    int alu_control, select_op_a;
    int carry_write_enable;
    int cmp_unsigned;

    int branch_enable;
    int branch_cond;

    address_t wb_dest_register;
    int wb_select_data;
    int wb_write_enable;

    int mem_enable;
    int mem_write_enable;
};

enum {
    EX_ALU_ADD = 0,
    EX_ALU_CMP = 1,
    EX_ALU_OR = 2,
    EX_ALU_AND = 3,
    EX_ALU_XOR = 4,
};

enum {
    EX_COND_EQ = 0,
    EX_COND_NE = 1,
    EX_COND_LT = 2,
    EX_COND_LE = 3,
    EX_COND_GT = 4,
    EX_COND_GE = 5,
    EX_COND_ALWAYS = 8,
};

extern struct ex_state_t ex_state;

void ex_stage(void);
