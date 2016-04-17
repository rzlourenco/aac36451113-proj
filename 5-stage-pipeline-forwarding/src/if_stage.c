#include "if_stage.h"

#include "id_stage.h"

#include "cpu_state.h"
#include "memory.h"

void if_stage(void) {
    if (cpu_state.id_stall) {
        return;
    }

    if (cpu_state.if_stalls > 0) {
        if (cpu_state.has_delayed_branch) {
            cpu_state.has_delayed_branch = 0;
            cpu_state.pc += sizeof(address_t);
        } else {
            cpu_state.id_enable = 0;
            return;
        }
    }

    if (mem_state.if_branch) {
        cpu_state.pc = mem_state.if_branch_target;
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
}
