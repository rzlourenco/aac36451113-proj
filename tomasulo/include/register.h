#pragma once

#include "rob.h"

#include "common.h"

NEWTYPE(reg, word_t)

rob_tag_t register_read(reg_t reg, word_t *data);

void register_write(reg_t reg, rob_tag_t tag);

void register_real_write(reg_t reg, word_t data);

reg_t reg_gpr(word_t reg);
reg_t reg_spr(word_t reg);
reg_t reg_flag(word_t reg);

enum {
    REGISTER_FLAG_CARRY,
};

void register_clock(void);
