#include <stdlib.h>
#include "cpu_state.h"
#include "if_stage.h"
#include "memory.h"

struct if_id_state_t if_id_state;
struct if_state_t if_state;

void if_stage(void) {
    word_t instruction = memory_read(cpu_state.pc);

    // ADD r0,r0,r0
    // Apparently should not be emitted by the compiler
    if (instruction == 0) {
        cpu_state.halt = 1;
        cpu_state.if_enable = 0;
        return;
    }

    if_id_state.instruction = instruction;
    if_id_state.pc = cpu_state.pc;

    switch (if_state.pc_sel) {
        case 0:
            cpu_state.pc = cpu_state.pc + (address_t)sizeof(address_t);
            break;
        case 1:
            cpu_state.pc = if_state.ex_pc;
            break;
        default:
            abort();
    }

    cpu_state.id_enable = 1;
}
