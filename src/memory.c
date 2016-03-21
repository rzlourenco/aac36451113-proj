#include "memory.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static word_t *mem = NULL;
static size_t memsize = 0;

static inline address_t real_address(address_t address);
static inline word_t reverse_endianness(word_t value);

void init_memory(size_t bits) {
    if (mem != NULL) {
        free(mem);
    }

    memsize = 1u << bits;
    assert(memsize > 0);
    mem = calloc(memsize, sizeof(*mem));

    assert(mem);
}

void flash_memory(char const *data, size_t count) {
    assert(mem);
    assert(memsize > 0);

    count = count > memsize ? memsize : count;

    memcpy(mem, data, count);
}

word_t memory_read(address_t address) {
    assert(mem != NULL && "memory not initialized!");

    // FIXME: ROM comes in big endian...
    word_t data = reverse_endianness(mem[real_address(address)]);

    return data;
}

void memory_write(address_t address, word_t data_in) {
    assert(mem != NULL && "memory not initialized!");

    // stdout memory-mapped device
    if (address == STDOUT_MMIO) {
        printf("%c", (char)data_in);
    } else {
        // FIXME: ROM comes in big endian...
        mem[real_address(address)] = reverse_endianness(data_in);
    }
}

static inline address_t real_address(address_t address) {
    address_t ret = address;

    // Remove non-addressable bits
    ret &= memsize - 1;

    // We only allow aligned accesses, so mask away bits
    // that would cause misalignments
    ret >>= log2i(sizeof(word_t));

    return ret;
}

static inline word_t reverse_endianness(word_t value) {
    assert(sizeof(word_t) == 4);

    return (value >>  0u & 0xFFu) << 24u |
           (value >>  8u & 0xFFu) << 16u |
           (value >> 16u & 0xFFu) <<  8u |
           (value >> 24u & 0xFFu) <<  0u;
}
