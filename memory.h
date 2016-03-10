#pragma once

#include "common.h"

#include <stddef.h>

int init_memory(size_t bits);
int flash_memory(address_t address, word_t const *data, size_t count);

word_t memory(word_t data_in, address_t address, int write_enable);
