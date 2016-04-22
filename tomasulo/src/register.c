#include "register.h"

#include <assert.h>

enum {
    REGISTER_COUNT = 32,
};

static struct {
    word_t data;
} registers[REGISTER_COUNT] = { 0 };

static word_t temp_registers[REGISTER_COUNT] = { 0 };

word_t register_read(address_t reg) {
    assert(reg < REGISTER_COUNT);

    return 0;
}

void register_write(address_t reg, word_t data) {
    assert(reg < REGISTER_COUNT);
}

void register_dump(void) {

}

void register_clock(void) {

}
