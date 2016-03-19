#include "ex_stage.h"
#include "id_stage.h"
#include "cpu_state.h"

#include <stdlib.h>

struct ex_mem_state_t ex_mem_state;

static int carry;

static word_t alu_add(word_t op_a, word_t op_b, word_t op_c) {
    static uint64_t mask = 0xFFFFFFFF00000000;
    uint64_t lop_a = (uint64_t)op_a,
             lop_b = (uint64_t)op_b,
             lop_c = (uint64_t)op_c,
             result;

    result = lop_a + lop_b + lop_c;
    carry = (result & mask) != 0;
    return (word_t)(result & ~mask);
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

    switch (id_ex_state.select_operation) {
        case ALU_CMP:
        case ALU_SUB:
            op_b = ~op_b;
        case ALU_ADD:
            ret = alu_add(op_a, op_b, id_ex_state.op_c);
            break;
        default:
            break;
    }

    msr.c = id_ex_state.keep_carry ? msr.c : carry;

    cpu_state.ex_enable = cpu_state.id_enable;
    cpu_state.mem_enable = 1;
}
