#pragma once

#include "common.h"

struct id_state_t {
    address_t pc;
    instruction_t instruction;

    int br_pred_taken;
    int br_pred_delayed;
    word_t br_pred_target;
};

extern struct id_state_t id_state;

void id_stage(void);
