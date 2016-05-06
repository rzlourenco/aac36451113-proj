#include "execute.h"

#include "cdb.h"
#include "cpu.h"
#include "issue.h"

enum {
    ALU_RS_COUNT = ISSUE_WIDTH,
};

struct rs_alu
w_rs_alu[ALU_RS_COUNT],
r_rs_alu[ALU_RS_COUNT];

int execute_queue_alu(struct rs_alu rs) {
    for (int i = 0; i < ALU_RS_COUNT; ++i) {
        if (w_rs_alu[i].busy)
            continue;

        w_rs_alu[i] = rs;
        return 0;
    }

    cpu_stats.sc_execute += 1;
    cpu_stats.sc_execute_alu += 1;
    return 1;
}

static struct {
    int carry;
    int negative;
    int zero;
} flags;

static word_t alu_add(word_t op_a, word_t op_b, word_t op_c);

static word_t alu_and(word_t op_a, word_t op_b);

static word_t alu_cmp(word_t op_a, word_t op_b, int is_signed);

static word_t alu_mul(word_t op_a, word_t op_b, int high, int op_a_unsigned, int op_b_unsigned);

static word_t alu_or(word_t op_a, word_t op_b);

static word_t alu_sext(word_t op_a, word_t op_b);

static word_t alu_sl(word_t op_a, word_t op_b);

static word_t alu_sr(word_t op_a, word_t op_b, int use_carry, int carry, int is_signed);

static word_t alu_xor(word_t op_a, word_t op_b);

static void
copy_front(void)
{
    for (int i = 0; i < ALU_RS_COUNT; ++i)
        r_rs_alu[i] = w_rs_alu[i];
}

static void
copy_back(void)
{
    for (int i = 0; i < ALU_RS_COUNT; ++i)
        w_rs_alu[i] = r_rs_alu[i];
}

static void
do_alu_op(void)
{
    for (int i = ALU_RS_COUNT-1; i >= 0; ++i) {
        if (!r_rs_alu[i].busy)
            continue;

        if (!rob_tag_eq(r_rs_alu[i].Qj, ROB_TAG_INVALID)
                || !rob_tag_eq(r_rs_alu[i].Qk, ROB_TAG_INVALID)
                || !rob_tag_eq(r_rs_alu[i].Ql, ROB_TAG_INVALID))
            continue;

        flags.carry = 0;
        flags.negative = 0;
        flags.zero = 0;

        word_t result = 0,
                op_a = r_rs_alu[i].Vj,
                op_b = r_rs_alu[i].Vk,
                op_c = r_rs_alu[i].Vl;

        switch (r_rs_alu[i].op) {
        case EX_ALU_ADD:
            result = alu_add(op_a, op_b, 0);
            break;
        case EX_ALU_ADDC:
            result = alu_add(op_a, op_b, op_c);
            break;
        case EX_ALU_RSUB:
            result = alu_add(~op_a, op_b, 1);
            break;
        case EX_ALU_RSUBC:
            result = alu_add(~op_a, op_b, op_c);
            break;
        case EX_ALU_CMP:
            result = alu_cmp(op_a, op_b, 1);
            break;
        case EX_ALU_CMPU:
            result = alu_cmp(op_a, op_b, 0);
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

        case EX_ALU_BSLL:
            result = alu_sl(op_a, op_b);
            break;
        case EX_ALU_BSRA:
            result = alu_sr(op_a, op_b, 0, 0, 1);
            break;
        case EX_ALU_BSRL:
            result = alu_sr(op_a, op_b, 0, 0, 0);
            break;
        case EX_ALU_SRA:
            result = alu_sr(op_a, 1, 0, 0, 1);
            break;
        case EX_ALU_SRC:
            result = alu_sr(op_a, 1, 1, op_c, 0);
            break;
        case EX_ALU_SRL:
            result = alu_sr(op_a, 1, 0, 0, 0);
            break;
        case EX_ALU_SEXT:
            result = alu_sext(op_a, op_b);
            break;

        case EX_ALU_EQ:
            result = op_a == op_b;
            break;
        case EX_ALU_NE:
            result = op_a != op_b;
            break;
        case EX_ALU_LT:
            result = op_a < op_b;
            break;
        case EX_ALU_LE:
            result = op_a <= op_b;
            break;
        case EX_ALU_GT:
            result = op_a > op_b;
            break;
        case EX_ALU_GE:
            result = op_a >= op_b;
            break;

        default:
            ABORT_WITH_MSG("Unknown ALU operation %d", r_rs_alu[i].op);
        }

        flags.negative = (result & 0x80000000) != 0;
        flags.zero = result == (l_word_t) 0;

        if (cdb_write(r_rs_alu[i].Qi, result))
            break;

        rob_get_entry(r_rs_alu[i].Qi)->new_carry = flags.carry;
        r_rs_alu[i].busy = 0;
        break;
    }
}

void execute_clock(void) {
    copy_front();

    // Get stuff from the CDB
    for (int i = 0; i < ALU_RS_COUNT; ++i) {
        if (r_rs_alu[i].busy) {
            if (!cdb_read(r_rs_alu[i].Qj, &r_rs_alu[i].Vj))
                r_rs_alu[i].Qj = ROB_TAG_INVALID;

            if (!cdb_read(r_rs_alu[i].Qk, &r_rs_alu[i].Vk))
                r_rs_alu[i].Qk = ROB_TAG_INVALID;

            if (!cdb_read(r_rs_alu[i].Ql, &r_rs_alu[i].Vl))
                r_rs_alu[i].Ql = ROB_TAG_INVALID;
        }
    }

    do_alu_op();

    copy_back();
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

static word_t alu_cmp(word_t op_a, word_t op_b, int is_signed) {
    word_t result = alu_add(op_a, ~op_b, 1);

    if (is_signed) {
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

static word_t alu_sr(word_t op_a, word_t op_b, int use_carry, int carry, int is_signed) {
    word_t result;

    // Mask away extra bits
    op_b = op_b & 0x1F;

    result = op_a >> op_b;

    if (use_carry && is_signed) {
        ABORT_WITH_MSG("cannot do arithmetic shift right with carry");
    }

    word_t sign = 0;
    if (use_carry)
        sign = carry != 0u;
    if (is_signed)
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
