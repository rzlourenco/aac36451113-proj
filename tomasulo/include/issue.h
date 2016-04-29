#pragma once

#include "common.h"

enum {
    ISSUE_WIDTH = 2,
};

int issue_queue_instruction(address_t pc, word_t instr, int predicted, address_t target);

void issue_clock(void);
