#pragma once

#include "common.h"

struct ex_state_t {
    address_t pc;

    int alu_control;
    word_t op_a, op_b;
    int sel_op_a, sel_op_b;
    int is_signed;
    int carry_write_enable;
    int use_carry;
    int carry;

    int branch_enable;
    int branch_cond;

    int branch_sel_op;
    word_t branch_op;

    address_t wb_dest_register;
    int wb_select_data;
    int wb_write_enable;

    int mem_access;
    int mem_mode;
    int mem_write_enable;
    word_t mem_data;
};

enum {
    EX_ALU_ADD,
    EX_ALU_CMP,
    EX_ALU_OR,
    EX_ALU_AND,
    EX_ALU_XOR,
    EX_ALU_SHIFT_LEFT,
    EX_ALU_SHIFT_RIGHT,
    EX_ALU_MUL,
    EX_ALU_MULH,
    EX_ALU_MULHU,
    EX_ALU_MULHSU,
    EX_ALU_ADDC,
    EX_ALU_SUB,
    EX_ALU_SUBC,
    EX_ALU_ANDN,
    EX_ALU_SEXT,
};

enum {
    EX_COND_ALWAYS,
    EX_COND_EQ,
    EX_COND_NE,
    EX_COND_LT,
    EX_COND_LE,
    EX_COND_GT,
    EX_COND_GE,
};

enum {
    EX_SELOP_IMM,
    EX_SELOP_PC,
    EX_SELOP_FWD_MEM,
    EX_SELOP_FWD_WB,
};

extern struct ex_state_t ex_state;

void ex_stage(void);
