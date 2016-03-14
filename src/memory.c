#include "memory.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

static inline word_t reverse_endianness(word_t value) {
    assert(sizeof(word_t) == 4);


    return ((uword_t)value >>  0u & 0xFFu) << 24u |
           ((uword_t)value >>  8u & 0xFFu) << 16u |
           ((uword_t)value >> 16u & 0xFFu) <<  8u |
           ((uword_t)value >> 24u & 0xFFu) <<  0u;
}

int flash_memory(address_t address, char const *data, size_t count) {
    if (mem == NULL) {
        return 1;
    }

    if (address + count >= memsize) {
        return 2;
    }

    word_t const *datap = (word_t const *)data;
    for (size_t i = 0; i < count; ++i) {
        mem[i] = reverse_endianness(datap[i]);
    }

    return 0;
}

static inline address_t real_address(address_t address) {
    return (address & (memsize - 1)) >> log2i(sizeof(word_t));
}

word_t memory_read(address_t address) {
    assert(mem != NULL && "memory not initialized!");

    word_t data = mem[real_address(address)];

    dprintf("memory_read(0x%08x [0x%08x]) = 0x%x\n", address, real_address(address), data);

    return data;
}

void memory_write(address_t address, word_t data_in, int write_enable) {
    assert(mem != NULL && "memory not initialized!");

    if (write_enable) {
        mem[real_address(address)] = data_in;
    }
}
