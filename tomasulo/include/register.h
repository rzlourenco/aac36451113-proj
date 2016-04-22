#pragma once

#include "common.h"

word_t register_read(address_t reg);
void register_write(address_t reg, word_t data);

void register_dump(void);

void register_clock(void);
