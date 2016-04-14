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
    wb_state.dest_register = mem_state.wb_dest_register;
    wb_state.data = mem_state.wb_data;
    wb_state.alu_result = mem_state.alu_result;

    if (mem_state.memory_access) {
        word_t address = mem_state.alu_result;
        word_t offset = 0, mask = 0;

        switch (mem_state.mode) {
            case MEM_BYTE:
                mask = 0xFFu;
                offset = 8 * (address & 0x00000003u);
                offset = little_endian ? offset : 24 - offset;
                break;
            case MEM_HALF:
                mask = 0xFFFFu;
                offset = 16 * (address & 0x00000001u);
                offset = little_endian ? offset : 16 - offset;
                break;
            case MEM_WORD:
                mask = 0xFFFFFFFFu;
                offset = address & 0x00000000u;
                break;
            default:
                ABORT_WITH_MSG("invalid memory mode");
        }

        word_t read_data = memory_read(address);
        word_t write_data = mem_state.data;

        wb_state.memory_out = (read_data & (mask << offset)) >> offset;

        write_data = (read_data & (~(mask << offset))) | ((write_data & mask) << offset);

        if (mem_state.write_enable) {
            if (has_mem_trace && address == mem_trace) {
                fprintf(stderr, "%08x: M[%08x] <- %08x\n", mem_state.pc, address, write_data);
            }
            memory_write(address, write_data);
        }
    }
}
