#include "memory.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

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

int flash_memory(address_t address, char const *data, size_t count) {
    if (mem == NULL) {
        return 1;
    }

    if (address + count >= memsize) {
        return 2;
    }

    memcpy(mem, data, count);

    return 0;
}

word_t memory_read(address_t address) {
    assert(mem != NULL && "memory not initialized!");

    // Ignore extra bits in the address
    size_t real_address = address & (memsize - 1);

    return mem[real_address];
}

void memory_write(address_t address, word_t data_in, int write_enable) {
    assert(mem != NULL && "memory not initialized!");

    // Ignore extra bits in the address
    size_t real_address = address & (memsize - 1);

    if (write_enable) {
        mem[real_address] = data_in;
    }
}
