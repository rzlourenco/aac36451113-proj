#include "register.h"

#include "wb_stage.h"
#include "mem_stage.h"
#include "ex_stage.h"

#include <assert.h>

static struct {
    word_t data;
    int in_ex;
    int in_mem;
    int in_wb;
} registers[32] = { 0 };

word_t register_read(address_t reg) {
    assert(reg < 32);

    return registers[reg].data;
}

void register_write(address_t reg, word_t data) {
    assert(reg < 32);

    if (reg != 0) {
        registers[reg].data = data;
    }
}

void register_dump(void) {
    for (int i = 0; i < 32; i += 4) {
        fprintf(stderr, "\t(%d%d%d)r%-2d = 0x%08x, ",
                registers[i+0].in_ex,
                registers[i+0].in_mem,
                registers[i+0].in_wb,
                i+0,
                registers[i+0].data);

        fprintf(stderr, "(%d%d%d)r%-2d = 0x%08x, ",
                registers[i+1].in_ex,
                registers[i+1].in_mem,
                registers[i+1].in_wb,
                i+1,
                registers[i+1].data);

        fprintf(stderr, "(%d%d%d)r%-2d = 0x%08x, ",
                registers[i+2].in_ex,
                registers[i+2].in_mem,
                registers[i+2].in_wb,
                i+2,
                registers[i+2].data);

        fprintf(stderr, "(%d%d%d)r%-2d = 0x%08x\n",
                registers[i+3].in_ex,
                registers[i+3].in_mem,
                registers[i+3].in_wb,
                i+3,
                registers[i+3].data);
    }
}

void register_clock(void) {
    for (unsigned i = 1; i < 32; ++i) {
        registers[i].in_wb = wb_state.write_enable && wb_state.dest_register == i;
        registers[i].in_mem = mem_state.wb_write_enable && mem_state.wb_dest_register == i;
        registers[i].in_ex = ex_state.wb_write_enable && ex_state.wb_dest_register == i;
    }
}

int register_in_use(address_t reg) {
    assert(reg < 32);

    return registers[reg].in_ex || registers[reg].in_mem; // || registers[reg].in_wb;
}

void register_mark_used(address_t reg) {
    assert(reg < 32);

    if (reg != 0) {
        registers[reg].in_ex = 1;
    }
}
