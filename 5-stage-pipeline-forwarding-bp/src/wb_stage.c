#include "wb_stage.h"

#include "cpu_state.h"
#include "register.h"

struct wb_state_t wb_state;

void wb_stage(void) {
    word_t data = wb_get_result();

    if (wb_state.write_enable) {
        if (trace_register && trace_register == wb_state.dest_register) {
            fprintf(stderr, "%08x: r%u <- %08x\n", wb_state.pc, wb_state.dest_register, data);
        }
        register_write(wb_state.dest_register, data);
    }

    cpu_state.total_instructions++;
}

word_t wb_get_result(void) {
    switch (wb_state.select_data) {
        case WB_SEL_PC:
            return wb_state.pc;
        case WB_SEL_EX:
            return wb_state.alu_result;
        case WB_SEL_MEM:
            return wb_state.memory_out;
        default:
            ABORT_WITH_MSG("unknown WB_SEL");
    }
}
