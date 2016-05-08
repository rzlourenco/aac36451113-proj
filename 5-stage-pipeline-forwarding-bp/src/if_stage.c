#include "if_stage.h"

#include "id_stage.h"

#include "branch_predictor.h"
#include "cpu_state.h"
#include "memory.h"

struct if_state_t if_state;

void if_stage(void) {
    if (cpu_state.id_stall) {
        return;
    }

    word_t instruction = memory_read(cpu_state.pc);

    word_t pred_target = 0;
    int pred_delayed = 0;
    int pred_taken = 0;

    if (!if_state.pred_delay_slot) {
        bp_branch_predict(cpu_state.pc, &pred_taken, &pred_delayed, &pred_target);
    }

    id_state.instruction = instruction;
    id_state.pc = cpu_state.pc;

    id_state.br_pred_taken = pred_taken;
    id_state.br_pred_delayed = pred_delayed;
    id_state.br_pred_target = pred_target;

    if (if_state.pred_delay_slot) {
        if_state.pred_delay_slot = 0;
        if_state.next_pc = if_state.branch_pc;
    } else if (pred_taken) {
        if (pred_delayed) {
            if_state.pred_delay_slot = 1;
            if_state.next_pc = cpu_state.pc + sizeof(cpu_state.pc);
            if_state.branch_pc = pred_target;
        }
        else {
            if_state.next_pc = pred_target;
        }
    } else {
        if_state.next_pc = cpu_state.pc + sizeof(cpu_state.pc);
    }

    if (has_breakpoint && cpu_state.pc == breakpoint) {
        debug = 1;
    }

    cpu_state.pc = if_state.next_pc;
}
