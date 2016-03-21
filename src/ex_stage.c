#include "ex_stage.h"
#include "id_stage.h"
#include "cpu_state.h"

#include <assert.h>
#include <mem_stage.h>

static int carry;

struct ex_state_t ex_state;

static word_t alu_cmp(word_t op_a, word_t op_b, int unsigned_compare);

static word_t alu_add(word_t op_a, word_t op_b, word_t op_c);

void ex_stage(void) {
    word_t op_a, op_b, op_c, result;

    switch (ex_state.select_op_a) {
        case 0:
            op_a = ex_state.op_a;
            break;
        case 1:
            op_a = ex_state.pc;
            break;
        default:
            ABORT_MSG("Unknown operand A select");
    }

    op_b = ex_state.op_b;
    op_c = ex_state.op_c;

    switch (ex_state.alu_control) {
        case EX_ALU_CMP:
            mem_state.alu_result = alu_cmp(op_a, op_b, ex_state.cmp_unsigned);
            break;
        case EX_ALU_ADD:
            mem_state.alu_result = alu_add(op_a, op_b, op_c);
            break;
        default:
            ABORT_MSG("Unknown ALU operation");
    }

    mem_state.wb_dest_register = ex_state.wb_dest_register;
    mem_state.wb_write_enable = ex_state.wb_write_enable;
    mem_state.pc = ex_state.pc;

    mem_state.write_enable = ex_state.mem_write_enable;
    mem_state.data = ex_state.op_a;
    mem_state.enable = ex_state.mem_enable;

    if (ex_state.carry_write_enable) {
        msr.c = carry;
    }

    cpu_state.ex_enable = cpu_state.id_enable;
    cpu_state.mem_enable = 1;
}

static word_t alu_add(word_t op_a, word_t op_b, word_t op_c) {
    uint64_t const mask = 0xFFFFFFFF00000000;
    uint64_t lop_a = (uint64_t) op_a, lop_b = (uint64_t) op_b, lop_c = (uint64_t) op_c, result;

    result = lop_a + lop_b + lop_c;
    carry = (result & mask) != 0;
    return (word_t) (result & ~mask);
}

static word_t alu_cmp(word_t op_a, word_t op_b, int unsigned_compare) {
    word_t result = alu_add(op_a, ~op_b, 1);

    if (unsigned_compare) {
        if ((uword_t) op_a < (uword_t) op_b) {
            result |= 1u << 31u;
        }
        else {
            result &= ~(1u << 31u);
        }
    }
    else {
        if (op_a < op_b) {
            result |= 1u << 31u;
        }
        else {
            result &= ~(1u << 31u);
        }
    }

    return result;
}
