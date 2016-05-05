#pragma once

#include "rob.h"

#include "common.h"

struct rs_alu {
    int op, busy;
    rob_tag_t Qj, Qk, Ql;
    word_t Vj, Vk, Vl;
};

enum {
    EX_ALU_ADD,
    EX_ALU_ADDC,
    EX_ALU_RSUB,
    EX_ALU_RSUBC,
    EX_ALU_CMP,
    EX_ALU_CMPU,
    EX_ALU_OR,
    EX_ALU_AND,
    EX_ALU_XOR,
    EX_ALU_ANDN,
    EX_ALU_SHIFT_LEFT,
    EX_ALU_SHIFT_RIGHT,
    EX_ALU_SEXT,
    EX_ALU_MUL,
    EX_ALU_MULH,
    EX_ALU_MULHU,
    EX_ALU_MULHSU,
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

void execute_clock(void);
