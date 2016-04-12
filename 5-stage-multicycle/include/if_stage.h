#pragma once

#include "common.h"

struct if_state_t {
    int pc_sel;

    address_t branch_pc;
    address_t next_pc;
};

enum {
    IF_SELPC_NEXT = 0,
    IF_SELPC_BRANCH = 1,
};

extern struct if_state_t if_state;

void if_stage(void);
