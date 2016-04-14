#pragma once

#include "common.h"

struct wb_state_t {
    address_t pc;
    word_t memory_out;
    word_t alu_result;

    address_t dest_register;
    int select_data;
    int write_enable;
};

enum {
    WB_SEL_PC = 0,
    WB_SEL_EX = 1,
    WB_SEL_MEM = 2,
};

extern struct wb_state_t wb_state;

void wb_stage(void);
