#include "ex_stage.h"
#include "id_stage.h"
#include "cpu_state.h"

#include <assert.h>
#include <mem_stage.h>
#include <if_stage.h>

struct ex_state_t ex_state;

static unsigned int carry;
static struct {
    word_t carry : 1;

    word_t negative : 1;
    word_t zero : 1;
} flags;

static word_t alu_add(word_t op_a, word_t op_b, word_t op_c);
static word_t alu_and(word_t op_a, word_t op_b);
static word_t alu_cmp(word_t op_a, word_t op_b, int unsigned_compare);
static word_t alu_or(word_t op_a, word_t op_b);
static word_t alu_xor(word_t op_a, word_t op_b);

void ex_stage(void) {
    word_t op_a, op_b, op_c, result;

    mem_state = (struct mem_state_t){ 0 };
    // These pass through
    mem_state.pc = ex_state.pc;
    mem_state.wb_dest_register = ex_state.wb_dest_register;
    mem_state.wb_write_enable = ex_state.wb_write_enable;
    mem_state.wb_select_data = ex_state.wb_select_data;
    mem_state.write_enable = ex_state.mem_write_enable;
    mem_state.enable = ex_state.mem_enable;

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
            result = alu_cmp(op_a, op_b, ex_state.cmp_unsigned);
            break;
        case EX_ALU_ADD:
            result = alu_add(op_a, op_b, op_c);
            break;
        case EX_ALU_OR:
            result = alu_or(op_a, op_b);
            break;
        case EX_ALU_AND:
            result = alu_and(op_a, op_b);
            break;
        case EX_ALU_XOR:
            result = alu_xor(op_a, op_b);
            break;
        default:
            ABORT_MSG("Unknown ALU operation");
    }

    mem_state.data = ex_state.op_a;

    if (ex_state.carry_write_enable) {
        msr.c = carry;
    }

    if (ex_state.branch_enable && (ex_state.branch_cond == EX_COND_ALWAYS ||
                                   (ex_state.branch_cond == EX_COND_EQ && flags.zero) ||
                                   (ex_state.branch_cond == EX_COND_NE && !flags.zero) ||
                                   (ex_state.branch_cond == EX_COND_LT && flags.negative) ||
                                   (ex_state.branch_cond == EX_COND_GT && !flags.negative) ||
                                   (ex_state.branch_cond == EX_COND_LE && (flags.zero || flags.negative)) ||
                                   (ex_state.branch_cond == EX_COND_GE && (flags.zero || !flags.negative)))) {
        if_state.branch_pc = result;
        if_state.pc_sel = IF_SELPC_BRANCH;
    }

    mem_state.alu_result = result;

    cpu_state.mem_enable = 1;
    cpu_state.ex_enable = 0;
}

static word_t alu_add(word_t op_a, word_t op_b, word_t op_c) {
    l_word_t const mask = 0xFFFFFFFF00000000;
    l_word_t lop_a = (l_word_t) op_a, lop_b = (l_word_t) op_b, lop_c = (l_word_t) op_c, result;

    result = lop_a + lop_b + lop_c;
    flags.carry = (result & mask) != (l_word_t)0;
    flags.negative = result & ((l_word_t)1 << 31);
    flags.zero = result == (l_word_t)0;
    return (word_t) (result & ~mask);
}

static word_t alu_and(word_t op_a, word_t op_b) {
    word_t result = op_a & op_b;

    flags.negative = result & ((l_word_t)1 << 31);
    flags.zero = result == (l_word_t)0;

    return result;
}

static word_t alu_cmp(word_t op_a, word_t op_b, int unsigned_compare) {
    word_t result = alu_add(op_a, ~op_b, 1);

    if (unsigned_compare) {
        if (op_a < op_b) {
            result |= 1u << 31u;
        }
        else {
            result &= ~(1u << 31u);
        }
    }
    else {
        if ((s_word_t) op_a < (s_word_t) op_b) {
            result |= 1u << 31u;
        }
        else {
            result &= ~(1u << 31u);
        }
    }

    return result;
}

static word_t alu_or(word_t op_a, word_t op_b) {
    word_t result = op_a | op_b;

    flags.negative = result & ((l_word_t)1 << 31);
    flags.zero = result == (l_word_t)0;

    return result;
}

static word_t alu_xor(word_t op_a, word_t op_b) {
    word_t result = op_a ^ op_b;

    flags.negative = result & ((l_word_t)1 << 31);
    flags.zero = result == (l_word_t)0;

    return result;
}
