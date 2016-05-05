#include "register.h"

#include "rob.h"

#include <assert.h>

enum {
    REGISTER_GPR_COUNT = 32,
    REGISTER_SPR_COUNT = 18,
    REGISTER_FLAG_COUNT = 1,

    REGISTER_COUNT = REGISTER_GPR_COUNT + REGISTER_SPR_COUNT + REGISTER_FLAG_COUNT,

    REGISTER_GPR_START = 0,
    REGISTER_SPR_START = REGISTER_GPR_START + REGISTER_GPR_COUNT,
    REGISTER_FLAG_START = REGISTER_SPR_START + REGISTER_SPR_COUNT,
};

static struct {
    rob_tag_t tag;
    word_t data;
}
registers_r[REGISTER_COUNT] = { 0 },
registers_w[REGISTER_COUNT] = { 0 };

rob_tag_t
register_read(reg_t reg, word_t *data)
{
    assert(reg_val(reg) < REGISTER_COUNT);

    *data = registers_r[reg_val(reg)].data;

    return registers_r[reg_val(reg)].tag;
}

void
register_write(reg_t reg, rob_tag_t tag)
{
    assert(reg_val(reg) < REGISTER_COUNT);

    registers_w[reg_val(reg)].tag = tag;
}

void
register_real_write(reg_t reg, word_t data)
{
    register_write(reg, ROB_TAG_INVALID);

    registers_w[reg_val(reg)].data = data;
}

reg_t
reg_gpr(word_t reg)
{
    assert(reg < REGISTER_GPR_COUNT);

    return make_reg(reg + REGISTER_GPR_START);
}

reg_t
reg_spr(word_t reg)
{
    assert(reg < REGISTER_SPR_COUNT);

    return make_reg(reg + REGISTER_SPR_START);
}

reg_t
reg_flag(word_t reg)
{
    assert(reg < REGISTER_FLAG_COUNT);

    return make_reg(reg + REGISTER_FLAG_START);
}

void
register_clock(void)
{
    for (int i = 0; i < REGISTER_COUNT; ++i) {
        registers_r[i] = registers_w[i];
    }
}
