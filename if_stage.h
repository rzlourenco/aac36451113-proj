#pragma once

#include "common.h"

struct if_id_state {
    address_t next_pc;
    instruction_t instruction;
};

extern struct if_id_state if_id_state;

void instruction_fetch(void);
