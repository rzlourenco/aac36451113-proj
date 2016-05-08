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
    WB_SEL_PC,
    WB_SEL_EX,
    WB_SEL_MEM,
};

extern struct wb_state_t wb_state;

void wb_stage(void);

word_t wb_get_result(void);

