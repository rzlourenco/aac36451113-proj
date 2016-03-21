#pragma once

#include "common.h"

struct mem_state_t {
    address_t pc;

    address_t wb_dest_register;
    int wb_select_data;
    int wb_write_enable;

    word_t data;
    word_t alu_result;
    int write_enable;
    int enable;
};

extern struct mem_state_t mem_state;

void mem_stage(void);
