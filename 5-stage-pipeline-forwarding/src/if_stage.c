#include "if_stage.h"

#include "id_stage.h"

#include "cpu_state.h"
#include "memory.h"

struct if_state_t if_state;

void if_stage(void) {
    if (cpu_state.id_stall) {
        return;
    }

    if (cpu_state.if_stalls > 0) {
        if (cpu_state.has_delayed_branch) {
            cpu_state.has_delayed_branch = 0;
        } else {
            cpu_state.id_enable = 0;
            return;
        }
    }

    switch (if_state.sel_pc) {
        case IF_SELPC_NEXT:
            cpu_state.pc = if_state.next_pc;
            break;
        case IF_SELPC_BRANCH:
            cpu_state.pc = if_state.branch_pc;
            break;
        default:
            ABORT_WITH_MSG("unknown IF_SELPC value %d", if_state.sel_pc);
    }

    if (has_breakpoint && cpu_state.pc == breakpoint) {
        debug = 1;
    }

    word_t instruction = memory_read(cpu_state.pc);

    id_state.instruction = instruction;
    id_state.pc = cpu_state.pc;

    // BRI 0, an infinite loop
    if (instruction == 0xB8000000) {
        cpu_state.if_enable = 0;
        cpu_state.halt = 1;
        return;
    }

    if_state.sel_pc = IF_SELPC_NEXT;
    if_state.next_pc = cpu_state.pc + sizeof(cpu_state.pc);
}
