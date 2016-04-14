#pragma once

#include "common.h"

struct ex_state_t {
    address_t pc;

    word_t op_a, op_b, op_c;
    int alu_control, select_op_a;
    int carry_write_enable;
    int is_signed;
    int use_carry;

    int branch_enable;
    int branch_cond;

    address_t wb_dest_register;
    int wb_select_data;
    int wb_write_enable;

    int mem_access;
    int mem_mode;
    int mem_write_enable;
    word_t mem_data;
};

enum {
    EX_ALU_ADD = 0,
    EX_ALU_CMP = 1,
    EX_ALU_OR = 2,
    EX_ALU_AND = 3,
    EX_ALU_XOR = 4,
    EX_ALU_SHIFT_LEFT = 5,
    EX_ALU_SHIFT_RIGHT = 6,
    EX_ALU_MUL = 7,
    EX_ALU_MULH = 8,
    EX_ALU_MULHU = 9,
    EX_ALU_MULHSU = 10,
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
