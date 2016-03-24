#pragma once

#include "common.h"

struct wb_state_t {
    address_t pc;
    word_t memory_out;
    word_t alu_result;
    word_t data;

    address_t dest_address;
    int select_data;
    int write_enable;
};

enum {
    WB_SEL_PC = 0,
    WB_SEL_EX = 1,
    WB_SEL_MEM = 2,
    WB_SEL_WB = 3,
};

extern struct wb_state_t wb_state;

void wb_stage(void);
