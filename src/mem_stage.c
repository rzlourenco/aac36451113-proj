#include "mem_stage.h"

#include "cpu_state.h"
#include "ex_stage.h"
#include "memory.h"
#include "wb_stage.h"

struct mem_state_t mem_state;

void mem_stage(void) {
    // These pass unchanged to the next stage
    wb_state.pc = mem_state.pc;
    wb_state.select_data = mem_state.wb_select_data;
    wb_state.write_enable = mem_state.wb_write_enable;
    wb_state.dest_address = mem_state.wb_dest_register;
    wb_state.data = mem_state.wb_data;
    wb_state.alu_result = mem_state.alu_result;

    word_t address = mem_state.alu_result;

    word_t offset = 0, mask = 0;
    switch (mem_state.mode) {
        case MEM_BYTE:
            mask = 0xFFu;
            offset = (address & 0x00000003u) * 8u;
            break;
        case MEM_HALF:
            mask = 0xFFFFu;
            offset = (address & 0x00000001u) * 16u;
            break;
        case MEM_WORD:
            mask = 0xFFFFFFFFu;
            offset = address & 0x00000000u;
            break;
        default:
            ABORT_WITH_MSG("invalid memory mode");
    }

    if (mem_state.memory_access) {

        word_t read_data = memory_read(address);
        word_t write_data = mem_state.data;

        wb_state.memory_out = (read_data & (mask << offset)) >> offset;

        write_data = (read_data & (~(mask << offset))) | ((write_data & mask) << offset);

        if (mem_state.write_enable) {
            memory_write(address, write_data);
        }
    }
}
