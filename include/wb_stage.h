#pragma once

#include "common.h"

struct wb_state_t {
    address_t pc;
    word_t memory_out;
    word_t alu_result;
    int sel_out;
    int write_enabled;
    int dest_address;
};

enum {
    WB_SEL_PC,
    WB_SEL_EX,
    WB_SEL_MEM,
};

extern struct wb_state_t wb_state;

void wb_stage(void);
