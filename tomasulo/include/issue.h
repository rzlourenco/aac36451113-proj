#pragma once

#include "common.h"

enum {
    ISSUE_WIDTH = 2,
};

int issue_fetch(address_t pc, word_t instr, int predicted, address_t target);

void issue_clock(void);
