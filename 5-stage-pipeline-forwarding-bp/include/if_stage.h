#pragma once

#include "common.h"

struct if_state_t {
    address_t branch_pc;
    address_t next_pc;
    int sel_pc;

    int pred_delay_slot;
    word_t pred_target;
};

enum {
    IF_SELPC_NEXT,
    IF_SELPC_BRANCH,
    IF_SELPC_DELAY_SLOT,
};

extern struct if_state_t if_state;

void if_stage(void);
