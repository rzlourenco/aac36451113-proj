#include "ex_stage.h"
#include "id_stage.h"
#include "cpu_state.h"

#include <assert.h>
#include <mem_stage.h>
#include <if_stage.h>

struct ex_state_t ex_state;

static struct {
    int carry;
    int negative;
    int zero;
} flags;

static word_t alu_add(word_t op_a, word_t op_b, word_t op_c);

static word_t alu_and(word_t op_a, word_t op_b);

static word_t alu_cmp(word_t op_a, word_t op_b);

static word_t alu_mul(word_t op_a, word_t op_b, int high, int op_a_unsigned, int op_b_unsigned);

static word_t alu_or(word_t op_a, word_t op_b);

static word_t alu_sext(word_t op_a, word_t op_b);

static word_t alu_sl(word_t op_a, word_t op_b);

static word_t alu_sr(word_t op_a, word_t op_b);

static word_t alu_xor(word_t op_a, word_t op_b);

static void branch_control(word_t branch_op, word_t alu_result);

void ex_stage(void) {
    word_t result, op_a, op_b, branch_op;

    switch (ex_state.sel_op_a) {
        case EX_SELOP_IMM:
            op_a = ex_state.op_a;
            break;
        case EX_SELOP_PC:
            op_a = ex_state.pc;
            break;
        default:
            ABORT_WITH_MSG("unknown EX_SELOP A value");
    }

    switch (ex_state.sel_op_b) {
        case EX_SELOP_IMM:
            op_b = ex_state.op_b;
            break;
        case EX_SELOP_PC:
            op_b = ex_state.pc;
            break;
        default:
            ABORT_WITH_MSG("unknown EX_SELOP B value");
    }

    switch (ex_state.branch_sel_op) {
        case EX_SELOP_IMM:
            branch_op = ex_state.branch_op;
            break;
        case EX_SELOP_PC:
            branch_op = ex_state.pc;
            break;
        default:
            ABORT_WITH_MSG("unknown EX_SELOP BRANCH value");
    }

    flags.carry = flags.negative = flags.zero = 0;

    // These pass through
    mem_state.pc = ex_state.pc;

    mem_state.wb_dest_register = ex_state.wb_dest_register;
    mem_state.wb_write_enable = ex_state.wb_write_enable;
    mem_state.wb_select_data = ex_state.wb_select_data;

    mem_state.write_enable = ex_state.mem_write_enable;
    mem_state.memory_access = ex_state.mem_access;
    mem_state.data = ex_state.mem_data;
    mem_state.mode = ex_state.mem_mode;

    // Disable control signals that change state
    mem_state.if_branch = 0;

    switch (ex_state.alu_control) {
    case EX_ALU_ADD:
        result = alu_add(op_a, op_b, 0);
        break;
    case EX_ALU_ADDC:
        result = alu_add(op_a, op_b, msr.c);
        break;
    case EX_ALU_SUB:
        result = alu_add(~op_a, op_b, 1);
        break;
    case EX_ALU_SUBC:
        result = alu_add(~op_a, op_b, msr.c);
        break;
    case EX_ALU_CMP:
        result = alu_cmp(op_a, op_b);
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
    case EX_ALU_ANDN:
        result = alu_and(op_a, ~op_b);
        break;
    case EX_ALU_SHIFT_LEFT:
        result = alu_sl(op_a, op_b);
        break;
    case EX_ALU_SHIFT_RIGHT:
        result = alu_sr(op_a, op_b);
        break;
    case EX_ALU_MUL:
        result = alu_mul(op_a, op_b, 0, 0, 0);
        break;
    case EX_ALU_MULH:
        result = alu_mul(op_a, op_b, 1, 0, 0);
        break;
    case EX_ALU_MULHU:
        result = alu_mul(op_a, op_b, 1, 1, 1);
        break;
    case EX_ALU_MULHSU:
        result = alu_mul(op_a, op_b, 1, 1, 0);
        break;
    case EX_ALU_SEXT:
        result = alu_sext(op_a, op_b);
        break;
    default:
        ABORT_WITH_MSG("Unknown ALU operation %d", ex_state.alu_control);
    }

    mem_state.alu_result = result;

    flags.negative = (result & 0x80000000) != 0;
    flags.zero = result == (l_word_t) 0;

    if (ex_state.carry_write_enable) {
        msr.c = flags.carry;
    }

    if (ex_state.branch_enable) {
        branch_control(branch_op, result);
    }
}

static word_t alu_add(word_t op_a, word_t op_b, word_t op_c) {
    l_word_t const mask = 0xFFFFFFFF00000000;
    l_word_t lop_a = (l_word_t) op_a, lop_b = (l_word_t) op_b, lop_c = (l_word_t) op_c, result;

    result = lop_a + lop_b + lop_c;
    flags.carry = (result & mask) != (l_word_t) 0;

    return (word_t) (result & ~mask);
}

static word_t alu_and(word_t op_a, word_t op_b) {
    word_t result = op_a & op_b;

    return result;
}

static word_t alu_cmp(word_t op_a, word_t op_b) {
    word_t result = alu_add(op_a, ~op_b, 1);

    if (ex_state.is_signed) {
        if ((s_word_t) op_a < (s_word_t) op_b) {
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

static word_t alu_mul(word_t op_a_, word_t op_b_, int high, int op_a_unsigned, int op_b_unsigned) {
    l_word_t result = 0;
    l_word_t op_a = op_a_unsigned ? op_a_ : (l_word_t)(s_word_t)op_a_;
    l_word_t op_b = op_b_unsigned ? op_b_ : (l_word_t)(s_word_t)op_b_;

    result = op_a * op_b;

    if (high) {
        result >>= 32;
    }

    return (word_t)result;
}

static word_t alu_or(word_t op_a, word_t op_b) {
    word_t result = op_a | op_b;

    return result;
}

static word_t alu_sext(word_t op_a, word_t op_b) {
    word_t result = (word_t)sign_extend(op_a, 32, op_b);

    return result;
}

static word_t alu_sl(word_t op_a, word_t op_b) {
    word_t result;

    // Mask away extra bits
    op_b = op_b & 0x1F;
    result = op_a << op_b;

    return result;
}

static word_t alu_sr(word_t op_a, word_t op_b) {
    word_t result;

    // Mask away extra bits
    op_b = op_b & 0x1F;

    result = op_a >> op_b;

    if (ex_state.use_carry && ex_state.is_signed) {
        ABORT_WITH_MSG("cannot do arithmetic shift right with carry");
    }

    word_t sign = 0;
    if (ex_state.use_carry)
        sign = msr.c;
    if (ex_state.is_signed)
        sign = (s_word_t)op_a < 0;

    word_t sign_mask = sign ? ~((~0u << op_b) >> op_b) : 0;
    result |= sign_mask;

    flags.carry = (op_a & (((word_t)1 << op_b) - 1)) != 0;

    return result;
}

static word_t alu_xor(word_t op_a, word_t op_b) {
    word_t result = op_a ^ op_b;

    return result;
}

static void branch_control(word_t branch_op, word_t alu_result) {
    int less_than = (s_word_t)branch_op < 0;
    int equal = branch_op == 0;

    if ((ex_state.branch_cond == EX_COND_ALWAYS) ||
        (ex_state.branch_cond == EX_COND_EQ && equal) ||
        (ex_state.branch_cond == EX_COND_NE && !equal) ||
        (ex_state.branch_cond == EX_COND_LT && less_than) ||
        (ex_state.branch_cond == EX_COND_GT && !less_than) ||
        (ex_state.branch_cond == EX_COND_LE && (equal || less_than)) ||
        (ex_state.branch_cond == EX_COND_GE && (equal || !less_than))) {

        mem_state.if_branch_target = alu_result;
        mem_state.if_branch = 1;
    }
}
