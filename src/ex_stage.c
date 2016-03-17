#include "ex_stage.h"
#include "id_stage.h"
#include "cpu_state.h"

#include <stdlib.h>

struct ex_mem_state_t ex_mem_state;

static int carry;

static word_t alu_add(word_t op_a, word_t op_b, word_t op_c) {
    static long_uword_t mask = 0xFFFFFFFF00000000;
    long_uword_t lop_a = (long_uword_t)op_a,
                 lop_b = (long_uword_t)op_b,
                 lop_c = (long_uword_t)op_c,
                 result;

    result = lop_a + lop_b + lop_c;
    carry = (result & mask) != 0;
    return result & ~mask;
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
        case ALU_CMP:
        case ALU_SUB:
            op_b = ~op_b;
        case ALU_ADD:
            ret = alu_add(op_a, op_b, op_c);
            break;


        default:
            break;
    }


    cpu_state.ex_enable = cpu_state.id_enable;
    cpu_state.mem_enable = 1;
}
