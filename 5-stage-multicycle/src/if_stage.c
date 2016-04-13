#include <stdlib.h>
#include "cpu_state.h"
#include "if_stage.h"
#include "memory.h"
#include "id_stage.h"

struct if_state_t if_state;

void if_stage(void) {
    switch (if_state.pc_sel) {
        case IF_SELPC_NEXT:
            cpu_state.has_delayed_branch = 0;
            cpu_state.pc = if_state.next_pc;
            if_state.next_pc = if_state.next_pc + sizeof(word_t);
            break;
        case IF_SELPC_BRANCH:
            cpu_state.pc = cpu_state.has_delayed_branch ? if_state.next_pc : if_state.branch_pc;
            if_state.next_pc = cpu_state.has_delayed_branch ? if_state.branch_pc : (if_state.branch_pc + sizeof(word_t));
            cpu_state.has_delayed_branch = 0;
            break;
        default:
            ABORT_WITH_MSG("unknown IF_SELPC value");
            break;
    }

    if (has_breakpoint && cpu_state.pc == breakpoint) {
        debug = 1;
    }

    word_t instruction = memory_read(cpu_state.pc);

    id_state.instruction = instruction;
    id_state.pc = cpu_state.pc;

    // BRI 0, an infinite loop
    if (instruction == 0xB8000000) {
        cpu_state.halt = 1;
        return;
    }

    if_state.pc_sel = IF_SELPC_NEXT;
}
