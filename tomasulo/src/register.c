#include "register.h"

#include <assert.h>

enum {
    GPR_COUNT = 32,
    SPR_COUNT = 18,
};

static struct {
    word_t tag;
    word_t data;
}
registers_r[GPR_COUNT + SPR_COUNT] = { 0 },
registers_w[GPR_COUNT + SPR_COUNT] = { 0 };

static word_t register_read(word_t reg, word_t *data) {
    *data = registers_r[reg].data;
    return registers_r[reg].tag;
}

word_t register_read_gpr(word_t reg, word_t *data) {
    assert(reg < GPR_COUNT);

    return register_read(reg, data);
}

word_t register_read_spr(word_t reg, word_t *data) {
    assert(reg < SPR_COUNT);

    return register_read(reg + GPR_COUNT, data);
}

void register_clock(void) {

}
