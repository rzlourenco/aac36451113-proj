#include "register.h"

#include "ex_stage.h"
#include "mem_stage.h"
#include "wb_stage.h"

#include <assert.h>

enum {
    REGISTER_COUNT = 32,
};

static struct {
    word_t data;
    struct register_status_t status;
    int is_load;
} registers[REGISTER_COUNT] = { 0 };

static word_t temp_registers[REGISTER_COUNT] = { 0 };

word_t register_read(address_t reg) {
    assert(reg < REGISTER_COUNT);

    return registers[reg].data;
}

void register_write(address_t reg, word_t data) {
    assert(reg < REGISTER_COUNT);

    if (reg != 0) {
        temp_registers[reg] = data;
    }
}

void register_dump(void) {
    for (int i = 0; i < REGISTER_COUNT; i += 4) {
        fprintf(stderr, "\t(%d%d%d)r%-2d = 0x%08x, ",
                registers[i+0].status.in_ex,
                registers[i+0].status.in_mem,
                registers[i+0].status.in_wb,
                i+0,
                registers[i+0].data);

        fprintf(stderr, "(%d%d%d)r%-2d = 0x%08x, ",
                registers[i+1].status.in_ex,
                registers[i+1].status.in_mem,
                registers[i+1].status.in_wb,
                i+1,
                registers[i+1].data);

        fprintf(stderr, "(%d%d%d)r%-2d = 0x%08x, ",
                registers[i+2].status.in_ex,
                registers[i+2].status.in_mem,
                registers[i+2].status.in_wb,
                i+2,
                registers[i+2].data);

        fprintf(stderr, "(%d%d%d)r%-2d = 0x%08x\n",
                registers[i+3].status.in_ex,
                registers[i+3].status.in_mem,
                registers[i+3].status.in_wb,
                i+3,
                registers[i+3].data);
    }
}

void register_clock(void) {
    for (unsigned i = 1; i < REGISTER_COUNT; ++i) {
        registers[i].data = temp_registers[i];

        registers[i].status.in_wb = wb_state.write_enable && wb_state.dest_register == i;
        registers[i].status.in_mem = mem_state.wb_write_enable && mem_state.wb_dest_register == i;
        registers[i].status.in_ex = ex_state.wb_write_enable && ex_state.wb_dest_register == i;

        registers[i].status.is_wb_load = registers[i].status.is_mem_load;
        registers[i].status.is_mem_load = registers[i].status.is_ex_load;
        registers[i].status.is_ex_load = registers[i].is_load;
        registers[i].is_load = 0;
    }
}

struct register_status_t register_status(address_t reg) {
    assert(reg < REGISTER_COUNT);

    return registers[reg].status;
}

void register_mark_load(address_t reg) {
    assert(reg < REGISTER_COUNT);

    if (reg == 0)
        return;

    registers[reg].is_load = 1;
}
