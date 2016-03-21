#pragma once

#include "common.h"

struct if_state_t {
    int pc_sel;
    address_t ex_pc;
};

enum {
    IF_SELPC_NEXT = 0,
    IF_SEL_PC_ID = 1,
    IF_SELPC_EX = 2,
};

extern struct if_state_t if_state;

void if_stage(void);
