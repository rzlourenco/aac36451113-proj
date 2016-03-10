#pragma once

#include "common.h"

struct mem_wb_state {
    address_t next_pc;
    word_t data_out;
    word_t s;
    int sel_out;
    int write_enabled;
    int dest_address;
};

extern struct mem_wb_state mem_wb_state;

void mem_stage(void);
