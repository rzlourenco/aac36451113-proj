#pragma once

#include "common.h"

struct id_state_t {
    address_t pc;
    instruction_t instruction;
};

extern struct id_state_t id_state;

void id_stage(void);
