#include "wb_stage.h"

#include "mem_stage.h"
#include "cpu_state.h"
#include "reg_file.h"

#include <stdlib.h>

struct wb_state_t wb_state;

void wb_stage(void) {
    word_t data;

    switch (wb_state.select_data) {
        case WB_SEL_PC:
            data = wb_state.pc;
            break;
        case WB_SEL_EX:
            data = wb_state.alu_result;
            break;
        case WB_SEL_MEM:
            data = wb_state.memory_out;
            break;
        default:
            ABORT_MSG("unknown WB_SEL");
    }

    if (wb_state.write_enabled) {
        reg_file_write(wb_state.dest_address, data);
    }

    cpu_state.wb_enable = 0;
    cpu_state.total_instructions++;
}
