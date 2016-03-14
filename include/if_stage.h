#pragma once

#include "common.h"

struct if_id_state_t {
    address_t pc;
    instruction_t instruction;
};

struct if_state_t {
    int pc_sel;
    address_t ex_pc;
};

extern struct if_id_state_t if_id_state;
extern struct if_state_t if_state;


void if_stage(void);
