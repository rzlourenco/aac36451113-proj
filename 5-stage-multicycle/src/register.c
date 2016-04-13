#include "register.h"

#include <assert.h>

static struct {
    word_t data;
} registers[32] = { 0 };

word_t register_read(address_t reg) {
    assert(reg < 32);

    return registers[reg].data;
}

void register_write(address_t reg, word_t data) {
    assert(reg < 32);

    if (reg != 0) {
        registers[reg].data = data;
    }
}

void register_dump(void) {
    for (int i = 0; i < 32; i += 4) {
        fprintf(stderr, "r%-2d = 0x%08x, ",
                i+0,
                registers[i+0].data);

        fprintf(stderr, "r%-2d = 0x%08x, ",
                i+1,
                registers[i+1].data);

        fprintf(stderr, "r%-2d = 0x%08x, ",
                i+2,
                registers[i+2].data);

        fprintf(stderr, "r%-2d = 0x%08x\n",
                i+3,
                registers[i+3].data);
    }
}
