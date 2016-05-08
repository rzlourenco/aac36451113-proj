#pragma once

#include "common.h"

struct mem_state_t {
    address_t pc;

    address_t wb_dest_register;
    int wb_select_data;
    int wb_write_enable;

    int if_branch;
    word_t if_branch_target;

    word_t data;
    word_t alu_result;
    int write_enable;
    int memory_access;
    int mode;
};

enum {
    MEM_BYTE,
    MEM_HALF,
    MEM_WORD,
};

extern struct mem_state_t mem_state;

void mem_stage(void);
