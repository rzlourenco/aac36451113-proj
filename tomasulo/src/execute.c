#include "execute.h"

#include "cdb.h"
#include "cpu.h"
#include "issue.h"

enum {
    ALU_RS_COUNT = ISSUE_WIDTH,
};

static struct rs_alu
alu_rs_tmp[ALU_RS_COUNT],
alu_rs[ALU_RS_COUNT];

size_t
alu_rs_used = 0,
alu_rs_tmp_used = 0;

int execute_queue_alu(struct rs_alu rs) {
    if (alu_rs_tmp_used + alu_rs_used == ALU_RS_COUNT) {
        cpu_stats.sc_execute += 1;
        cpu_stats.sc_execute_alu += 1;
        return 1;
    }

    assert(alu_rs_tmp[alu_rs_tmp_used].busy == 0);
    alu_rs_tmp[alu_rs_tmp_used] = rs;
    alu_rs_tmp_used += 1;

    return 0;
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
copy_back(void)
{
    for (int i = 0; i < ALU_RS_COUNT; ++i)
        alu_rs_tmp[i] = alu_rs[i];
}

static void
do_alu_op(void)
{
    for (int i = 0; i < ALU_RS_COUNT; ++i) {
        if (!alu_rs[i].busy)
            continue;

        if (0
                || !rob_tag_eq(alu_rs[i].Qj, ROB_TAG_INVALID)
                || !rob_tag_eq(alu_rs[i].Qk, ROB_TAG_INVALID)
                || !rob_tag_eq(alu_rs[i].Ql, ROB_TAG_INVALID))
            continue;

        flags.carry = 0;
        flags.negative = 0;
        flags.zero = 0;

        word_t result = 0,
                op_a = alu_rs[i].Vj,
                op_b = alu_rs[i].Vk,
                op_c = alu_rs[i].Vl;

        switch (alu_rs[i].op) {
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
            ABORT_WITH_MSG("Unknown ALU operation %d", alu_rs[i].op);
        }

        flags.negative = (result & 0x80000000) != 0;
        flags.zero = result == (l_word_t) 0;

        if (cdb_write(alu_rs[i].Qi, result))
            break;

        rob_get_entry(alu_rs[i].Qi)->busy = 0;
        rob_get_entry(alu_rs[i].Qi)->new_carry = flags.carry;
        alu_rs[i].busy = 0;
        alu_rs_used -= 1;
        break;
    }
}

static void handle_alu(void);

void
execute_clock(void)
{
    handle_alu();
}

static void
handle_alu(void)
{
    // Get stuff from the CDB
    for (int i = 0; i < ALU_RS_COUNT; ++i) {
        if (alu_rs[i].busy) {
            if (!cdb_read(alu_rs[i].Qj, &alu_rs[i].Vj))
                alu_rs[i].Qj = ROB_TAG_INVALID;

            if (!cdb_read(alu_rs[i].Qk, &alu_rs[i].Vk))
                alu_rs[i].Qk = ROB_TAG_INVALID;

            if (!cdb_read(alu_rs[i].Ql, &alu_rs[i].Vl))
                alu_rs[i].Ql = ROB_TAG_INVALID;
        }
    }

    do_alu_op();

    for (size_t i = 0; i < alu_rs_tmp_used; ++i) {
        int found = 0;
        size_t ix;
        for (ix = 0; ix < ALU_RS_COUNT; ++ix) {
            if (alu_rs[ix].busy == 0) {
                found = 1;
                break;
            }
        }

        assert(found != 0);
        alu_rs[ix] = alu_rs_tmp[i];
        alu_rs_tmp[i] = (struct rs_alu){0};
        alu_rs_used += 1;
    }

    alu_rs_tmp_used = 0;
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

void
execute_dump(void)
{
    for (size_t i = 0; i < ALU_RS_COUNT; ++i) {
        fprintf(stderr, "[execute] alu(%d) op:%d Qi:%u Qj:%u Qk:%u Ql:%u Vj:%08x Vk:%08x Vl:%08x\n",
                alu_rs[i].busy,
                alu_rs[i].op,
                rob_tag_val(alu_rs[i].Qi),
                rob_tag_val(alu_rs[i].Qj),
                rob_tag_val(alu_rs[i].Qk),
                rob_tag_val(alu_rs[i].Ql),
                alu_rs[i].Vj,
                alu_rs[i].Vk,
                alu_rs[i].Vl
        );
    }

}
