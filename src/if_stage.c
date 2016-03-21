#include <stdlib.h>
#include "cpu_state.h"
#include "if_stage.h"
#include "memory.h"
#include "id_stage.h"

struct if_state_t if_state;

void if_stage(void) {
    word_t instruction = memory_read(cpu_state.pc);

    // BRI 0, an infinite loop
    if (instruction == 0xB8000000) {
        cpu_state.if_enable = 0;
        return;
    }

    id_state.instruction = instruction;
    id_state.pc = cpu_state.pc;

    switch (if_state.pc_sel) {
        case IF_SELPC_NEXT:
            cpu_state.pc = cpu_state.pc + (address_t) sizeof(address_t);
            break;
        case IF_SELPC_EX:
            cpu_state.pc = if_state.ex_pc;
            break;
        default:
            abort();
    }

    cpu_state.id_enable = 1;
}
