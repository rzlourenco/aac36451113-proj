#include "memory.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static word_t *mem = NULL;
static address_t memsize = 0;

static inline address_t real_address(address_t address);
static inline word_t reverse_endianness(word_t value);

void init_memory(size_t bits) {
    if (mem != NULL) {
        free(mem);
    }

    memsize = 1u << bits;
    mem = calloc(memsize, sizeof(*mem));

    assert(mem);
}

void flash_memory(address_t address, char const *data, size_t count) {
    assert(mem);
    assert(count % sizeof(word_t) == 0);
    assert(address + count <= memsize);

    address = real_address(address);

    // FIXME: ROM comes in big endian...
    for (address_t offset = 0; offset < count; offset += 1) {
        word_t word = ((word_t *)data)[offset];
        mem[offset + address] = reverse_endianness(word);
    }
}

word_t memory_read(address_t address) {
    assert(mem != NULL && "memory not initialized!");

    word_t data = mem[real_address(address)];

    dprintf("memory_read(0x%08x [0x%08x]) = 0x%x\n", address, real_address(address), data);

    return data;
}

void memory_write(address_t address, word_t data_in) {
    assert(mem != NULL && "memory not initialized!");

    dprintf("memory_write(0x%08x [0x%08x], 0x%08x)", address, real_address(address), data_in);

    // stdout memory-mapped device
    if (address == 0xFFFFFFC0) {
        printf("%c", (char)data_in);
    } else {
        mem[real_address(address)] = data_in;
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

    return ((uword_t)value >>  0u & 0xFFu) << 24u |
           ((uword_t)value >>  8u & 0xFFu) << 16u |
           ((uword_t)value >> 16u & 0xFFu) <<  8u |
           ((uword_t)value >> 24u & 0xFFu) <<  0u;
}
