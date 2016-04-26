#pragma once

#include "common.h"

#include <stddef.h>

void init_memory(size_t bits);

void flash_memory(char const *data, size_t count);

word_t memory_read(address_t address);

void memory_write(address_t address, word_t data_in);

void memory_clock(void);
