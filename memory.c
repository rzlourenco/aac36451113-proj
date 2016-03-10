#include "memory.h"

#include <assert.h>
#include <stdlib.h>

static word_t *mem = NULL;
static size_t memsize = 0;

int init_memory(size_t bits) {
    if (mem != NULL) {
        free(mem);
    }

    memsize = 1u << bits;
    mem = calloc(memsize, sizeof(*mem));

    if (mem == NULL) {
        return -1;
    }

    return 0;
}

int flash_memory(address_t address, word_t const *data, size_t count) {
    if (mem == NULL) {
        return -1;
    }

    if (address_val(address) + count >= memsize) {
        return -2;
    }

    for (size_t i = address_val(address); i < address_val(address) + count; ++i) {
        mem[i] = data[i];
    }

    return 0;
}

word_t memory(word_t data_in, address_t address, int write_enable) {
    assert(mem != NULL && "memory not initialized!");

    // Ignore extra bits in the address
    size_t real_address = address_val(address) & (memsize - 1);
    word_t word = mem[real_address];

    if (write_enable) {
        mem[real_address] = data_in;
    }

    return word;
}
