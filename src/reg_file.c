#include "reg_file.h"

#include <assert.h>

static word_t registers[32] = { 0 };

word_t reg_file_read(address_t address) {
    assert(address < 32);

    return registers[address];
}

void reg_file_write(address_t address, word_t data) {
    assert(address < 32);

    if (address != 0) {
        registers[address] = data;
    }
}

void reg_file_dump(void) {
    for (int i = 0; i < 32; i += 4) {
        fprintf(stderr, "\tr%-2d = 0x%08X, ", i+0, registers[i+0]);
        fprintf(stderr,   "r%-2d = 0x%08X, ", i+1, registers[i+1]);
        fprintf(stderr,   "r%-2d = 0x%08X, ", i+2, registers[i+2]);
        fprintf(stderr,   "r%-2d = 0x%08X\n", i+3, registers[i+3]);
    }
}
