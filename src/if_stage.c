#include <stdlib.h>
#include "cpu_state.h"
#include "if_stage.h"
#include "memory.h"
#include "id_stage.h"

struct if_state_t if_state;

void if_stage(void) {
    if (cpu_state.if_stalls > 0) {
        cpu_state.if_stalls--;

        if (!cpu_state.has_delayed_branch) {
            cpu_state.has_delayed_branch = 0;
            return;
        }
    }

    switch (if_state.pc_sel) {
        case IF_SELPC_NEXT:
            cpu_state.pc = if_state.next_pc;
            break;
        case IF_SELPC_BRANCH:
            cpu_state.pc = if_state.branch_pc;
            break;
        default:
            ABORT_WITH_MSG("unknown IF_SELPC value");

    }

    word_t instruction = memory_read(cpu_state.pc);

    // BRI 0, an infinite loop
    if (instruction == 0xB8000000) {
        cpu_state.if_enable = 0;
        return;
    }

    id_state.instruction = instruction;
    id_state.pc = cpu_state.pc;

    if_state.next_pc = cpu_state.pc + (address_t) sizeof(address_t);
    if_state.pc_sel = IF_SELPC_NEXT;

    cpu_state.id_enable = !cpu_state.id_stall;
}
