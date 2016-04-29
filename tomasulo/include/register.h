#pragma once

#include "common.h"

word_t register_read_gpr(word_t reg, word_t *data);

word_t register_read_spr(word_t reg, word_t *data);

enum {
    REGISTER_SPR_MSR,
};

void register_clock(void);
