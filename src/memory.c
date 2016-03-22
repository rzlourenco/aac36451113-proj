#include "memory.h"

#include "cpu_state.h"

#include <assert.h>
#include <string.h>

static word_t *mem = NULL;
static size_t memsize = 0;

static inline address_t real_address(address_t address);
static inline word_t handle_endianness(word_t value);

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
    if (mem == NULL) {
        ABORT_WITH_MSG("memory not initialized");
    }

    count = count > memsize ? memsize : count;

    memcpy(mem, data, count);
}

word_t memory_read(address_t address) {
    if (mem == NULL) {
        ABORT_WITH_MSG("memory not initialized");
    }

    word_t data = handle_endianness(mem[real_address(address)]);

    return data;
}

void memory_write(address_t address, word_t data_in) {
    assert(mem != NULL && "memory not initialized!");

    // stdout memory-mapped device
    if (address == STDOUT_MMIO) {
        printf("%c", (char)data_in);
    } else {
        mem[real_address(address)] = handle_endianness(data_in);
    }
}

static inline address_t real_address(address_t address) {
    address_t ret = address;

    // Remove non-addressable bits
    ret &= memsize - 1;

    // We only allow aligned accesses, so mask away bits
    // that would cause misalignments
    ret /= sizeof(word_t);

    return ret;
}

static inline word_t handle_endianness(word_t value) {
    assert(sizeof(word_t) == 4);

    if (little_endian) {
        return value;
    } else {
        return (value >> 0u & 0xFFu) << 24u |
               (value >> 8u & 0xFFu) << 16u |
               (value >> 16u & 0xFFu) << 8u |
               (value >> 24u & 0xFFu) << 0u;
    }
}
