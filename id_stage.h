#pragma once

#include "common.h"

struct id_ex_state {
    address_t next_pc;
    word_t op_a, op_b;
    int sel_s, sel_t;
    int cond;
    int imm;
    int op_sel;
    int mem_write;
    int sel_out;
    int dest_address;
    int write_enabled;
};

extern struct id_ex_state id_ex_state;

void id_stage(void);
