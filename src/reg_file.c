#include "reg_file.h"

#include <assert.h>

static word_t registers[32] = { { 0 } };

word_t reg_file_read(address_t address) {
    assert(address < ARRAY_LENGTH(registers));

    return registers[address];
}

void reg_file_write(address_t address, word_t data, int write_enable) {
    assert(address < ARRAY_LENGTH(registers));

    if (write_enable && address != 0) {
        registers[address] = data;
    }
}
