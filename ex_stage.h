#pragma once

#include "common.h"

struct ex_mem_state {
    address_t next_pc;
    int cond;
    word_t a;
    word_t s;
    int mem_write;
    int sel_out;
    int write_enabled;
    int dest_address;
};

extern struct ex_mem_state ex_mem_state;

void execute(void);