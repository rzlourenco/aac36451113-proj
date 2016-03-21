#pragma once

#include "common.h"

struct mem_state_t {
    address_t pc;
    int cond;
    word_t data;
    word_t alu_result;
    int write_enable;
    int sel_out;
    int wb_write_enable;
    int wb_dest_register;

    int enable;
};

extern struct mem_state_t mem_state;

void mem_stage(void);
