#pragma once

#include "common.h"

void reg_file(address_t aa, word_t *a,
              address_t ba, word_t *b,
              address_t da, word_t d,
              int write_enabled);

word_t register_read(address_t reg);
void register_write(address_t reg, word_t data);

void register_dump(void);

void register_clock(void);

int register_in_use(address_t reg);
void register_mark_used(address_t reg);
