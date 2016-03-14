#include "ex_stage.h"
#include "id_stage.h"
#include "cpu_state.h"

#include <stdlib.h>

struct ex_mem_state_t ex_mem_state;

static int carry;

static word_t alu_add(word_t op_a, word_t op_b, word_t op_c) {

}

void ex_stage(void) {
    word_t op_a, op_b, op_c, ret;

    switch (id_ex_state.select_op_a) {
        case 0:
            op_a = id_ex_state.op_a;
            break;
        case 1:
            op_a = id_ex_state.pc;
            break;
        default:
            abort();
    }

    switch (id_ex_state.select_op_b) {
        case 0:
            op_b = id_ex_state.op_b;
            break;
        case 1:
            op_b = id_ex_state.imm;
            break;
        default:
            abort();
    }

    op_c = 0;
    switch (id_ex_state.select_operation) {
        case ALU_ADD:
            ret = alu_add(op_a, op_b, op_c);
            break;


        default:
            break;
    }

}
