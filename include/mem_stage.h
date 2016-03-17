#pragma once

#include "common.h"

struct mem_wb_state_t {
    address_t pc;
    word_t memory_out;
    word_t alu_result;
    int sel_out;
    int write_enabled;
    int dest_address;
};

extern struct mem_wb_state_t mem_wb_state;

void mem_stage(void);
