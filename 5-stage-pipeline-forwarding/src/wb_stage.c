#include "wb_stage.h"

#include "cpu_state.h"
#include "register.h"

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
        case WB_SEL_WB:
            data = wb_state.data;
            break;
        default:
            ABORT_WITH_MSG("unknown WB_SEL");
    }

    if (wb_state.write_enable) {
        if (trace_register && trace_register == wb_state.dest_register) {
            fprintf(stderr, "%08x: r%u <- %08x\n", wb_state.pc, wb_state.dest_register, data);
        }
        register_write(wb_state.dest_register, data);
    }

    cpu_state.total_instructions++;
}
