#pragma once

#include "common.h"

#include <stddef.h>

int init_memory(size_t bits);

int flash_memory(address_t address, char const *data, size_t count);

word_t memory_read(address_t address);

void memory_write(address_t address, word_t data_in, int write_enable);
