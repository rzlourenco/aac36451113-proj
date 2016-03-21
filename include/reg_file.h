#pragma once

#include "common.h"

void reg_file(address_t aa, word_t *a,
              address_t ba, word_t *b,
              address_t da, word_t d,
              int write_enabled);

word_t reg_file_read(address_t address);
void reg_file_write(address_t address, word_t data);

void reg_file_dump(void);
