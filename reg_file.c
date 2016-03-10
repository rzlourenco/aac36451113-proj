#include "reg_file.h"

#include <assert.h>

static word_t registers[32] = { 0 };

void reg_file(address_t aa, word_t *a,
              address_t ba, word_t *b,
              address_t da, word_t d,
              int write_enabled) {
    assert(address_val(aa) < sizeof(registers));
    assert(address_val(ba) < sizeof(registers));
    assert(address_val(da) < sizeof(registers));

    *a = registers[address_val(aa)];
    *b = registers[address_val(ba)];

    // R0 is always 0
    if (write_enabled && address_val(da) != 0) {
        registers[address_val(da)] = d;
    }
}
