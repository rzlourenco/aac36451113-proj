#include "dispatch.h"

#include "cpu.h"
#include "execute.h"
#include "issue.h"
#include "register.h"
#include "rob.h"

#include <assert.h>

enum {
    QUEUE_SIZE = ISSUE_WIDTH,
};

static struct taginstr {
    rob_tag_t tag;
    address_t pc;
    word_t instr;
}
tmp[QUEUE_SIZE],
queue[QUEUE_SIZE];

static size_t
tmp_head = 0,
tmp_tail = 0,
tmp_size = 0,
queue_head = 0,
queue_tail = 0,
queue_size = 0;

int
dispatch_queue_instruction(rob_tag_t tag, address_t pc, word_t instr)
{
    if (tmp_size + queue_size == QUEUE_SIZE) {
        cpu_stats.sc_dispatch += 1;
        return 1;
    }

    tmp[tmp_tail].tag = tag;
    tmp[tmp_tail].pc = pc;
    tmp[tmp_tail].instr = instr;

    tmp_size += 1;
    tmp_tail = (tmp_tail + 1) % QUEUE_SIZE;

    return 0;
}

union mb_instr {
    struct {
        word_t func: 11;
        word_t rb: 5;
        word_t ra: 5;
        word_t rd: 5;
        word_t opcode: 6;
    };
    struct {
        // func
        word_t _0: 1;
        word_t u: 1;
        word_t _1: 7;
        word_t t: 1;
        word_t s: 1;

        // rb
        word_t _2: 5;

        // ra
        word_t _3: 2;
        word_t link: 1;
        word_t absolute: 1;
        word_t delayed_absolute: 1;

        // rd
        word_t _4: 4;
        word_t delayed_conditional: 1;

        // opcode

        word_t _5: 1;
        word_t use_carry: 1;
        word_t keep_carry: 1;
        word_t has_immediate: 1;
        word_t _6: 2;

    };
    struct {
        word_t imm16: 16;
        word_t _7: 16;
    };
    struct {
        word_t imm5: 5;
        word_t _8: 16;
        word_t cond: 3;
        word_t _9: 8;
    };
    word_t raw;
};

static struct taginstr *top(void);

static void push_instruction(void);
static void pop_instruction(void);

static int dispatch_arithmetic(rob_tag_t tag, word_t rawinstr);
static int dispatch_logical(rob_tag_t tag, word_t rawinstr);
static int dispatch_memory(rob_tag_t tag, word_t rawinstr);
static int dispatch_branch(rob_tag_t tag, address_t pc, word_t rawinstr);
static int dispatch_other(rob_tag_t tag, word_t rawinstr);

void
dispatch_clock(void)
{
    for (int i = 0; i < ISSUE_WIDTH; ++i) {
        if (queue_size == 0)
            break;

        if (!dispatch_arithmetic(top()->tag, top()->instr))
            pop_instruction();
        else if (!dispatch_logical(top()->tag, top()->instr))
            pop_instruction();
        else if (!dispatch_memory(top()->tag, top()->instr))
            pop_instruction();
        else if (!dispatch_branch(top()->tag, top()->pc, top()->instr))
            pop_instruction();
        else if (!dispatch_other(top()->tag, top()->instr))
            pop_instruction();
        else
            ABORT_WITH_MSG("illegal instruction %08x", top()->instr);
    }

    while (tmp_size > 0)
        push_instruction();

    tmp_head = 0;
    tmp_tail = 0;
}

void
dispatch_dump(void)
{
    fprintf(stderr, "[dispatch] th=%u tt=%u tz=%u\n",
            tmp_head,
            tmp_tail,
            tmp_size
    );

    for (int i = 0; i < tmp_size; ++i) {
        size_t ix = (tmp_head + i) % QUEUE_SIZE;
        fprintf(stderr, "[dispatch]     T %d@%zu pc:%08x R:%u\n",
                i,
                ix,
                tmp[ix].pc,
                rob_tag_val(tmp[ix].tag)
        );
    }

    fprintf(stderr, "[dispatch] qh=%u qt=%u qz=%u\n",
            queue_head,
            queue_tail,
            queue_size
    );

    for (int i = 0; i < queue_size; ++i) {
        size_t ix = (queue_head + i) % QUEUE_SIZE;
        fprintf(stderr, "[dispatch]     Q %d@%zu pc:%08x R:%u\n",
                i,
                ix,
                queue[ix].pc,
                rob_tag_val(queue[ix].tag)
        );
    }
}

static struct taginstr *
top(void)
{
    assert(queue_size > 0);

    return &queue[queue_head];
}

static void
push_instruction(void)
{
    assert(tmp_size > 0);
    assert(queue_size < QUEUE_SIZE);

    queue[queue_tail] = tmp[tmp_head];

    tmp_head = (tmp_head + 1) % QUEUE_SIZE;
    tmp_size -= 1;

    queue_tail = (queue_tail + 1) % QUEUE_SIZE;
    queue_size += 1;
}

static void
pop_instruction(void)
{
    assert(queue_size > 0);

    queue_head = (queue_head + 1) % QUEUE_SIZE;
    queue_size -= 1;
}

static rob_tag_t
extend_immediate(word_t immediate, word_t *data)
{
    // TODO
    *data = immediate;
    return ROB_TAG_INVALID;
}

static int
dispatch_arithmetic(rob_tag_t tag, word_t rawinstr)
{
    union mb_instr instr = { .raw = rawinstr };
    struct rs_alu rs = {0};

    rs.busy = 1;
    rs.Qi = tag;
    rs.Qj = register_read(reg_gpr(instr.ra), &rs.Vj);

    if (instr.has_immediate)
        rs.Qk = extend_immediate(instr.imm16, &rs.Vk);
    else
        rs.Qk = register_read(reg_gpr(instr.rb), &rs.Vk);

    if (instr.use_carry)
        rs.Ql = register_read(reg_flag(REGISTER_FLAG_CARRY), &rs.Vl);

    switch (instr.opcode) {
        case 0x00: // ADD
        case 0x04: // ADDK
        case 0x08: // ADDI
        case 0x0C: // ADDIK
            rs.op = EX_ALU_ADD;
            break;

        case 0x01: // RSUB
        case 0x05: // RSUBK/CMP/CMPU
        case 0x09: // RSUBI
        case 0x0D: // RSUBIK
            rs.op = EX_ALU_RSUB;
            break;

        case 0x02: // ADDC
        case 0x06: // ADDKC
        case 0x0A: // ADDIC
        case 0x0E: // ADDIKC
            rs.op = EX_ALU_ADDC;
            break;

        case 0x03: // RSUBC
        case 0x07: // RSUBKC
        case 0x0B: // RSUBIC
        case 0x0F: // RSUBIKC
            rs.op = EX_ALU_RSUBC;
            break;

        case 0x10: // MUL, MULH, MULHU, MULHSU
            switch (instr.func) {
                case 0x000:
                    rs.op = EX_ALU_MUL;
                    break;
                case 0x001:
                    rs.op = EX_ALU_MULH;
                    break;
                case 0x002:
                    rs.op = EX_ALU_MULHSU;
                    break;
                case 0x003:
                    rs.op = EX_ALU_MULHU;
                    break;

                default:
                    ABORT_WITH_MSG("invalid mul instruction (instr:%08x)", instr.raw);
            }
            break;

        case 0x18: // MULI
            rs.op = EX_ALU_MUL;
            break;

        default:
            return 1;
    }

    // We're dealing with a CMP/CMPU instruction
    if (instr.opcode == 0x05) {
        if (instr.func == 1)
            rs.op = EX_ALU_CMP;
        else if (instr.func == 3)
            rs.op = EX_ALU_CMPU;
        else
            ABORT_WITH_MSG("opcode 0x05 func %03x", instr.func);
    }

    if (execute_queue_alu(rs))
        return 1;

    if (!instr.keep_carry && instr.opcode != 0x10 && instr.opcode != 0x18) {
        rob_get_entry(tag)->write_carry = 1;
        register_write(reg_flag(REGISTER_FLAG_CARRY), tag);
    }

    register_write(reg_gpr(instr.rd), tag);

    return 0;
}

static int
dispatch_logical(rob_tag_t tag, word_t rawinstr)
{
    union mb_instr instr = { .raw = rawinstr };
    struct rs_alu rs = {0};
    int write_carry = 0;

    rs.busy = 1;
    rs.Qi = tag;
    rs.Qj = register_read(reg_gpr(instr.ra), &rs.Vj);

    if (instr.has_immediate)
        rs.Qk = extend_immediate(instr.imm16, &rs.Vk);
    else
        rs.Qk = register_read(reg_gpr(instr.rb), &rs.Vk);

    switch (instr.opcode) {
        case 0x11:
        case 0x19:
            if (instr.has_immediate)
                rs.Vk = instr.imm5;

            if (instr.s)
                rs.op = EX_ALU_BSLL;
            else if (instr.t)
                rs.op = EX_ALU_BSRA;
            else
                rs.op = EX_ALU_BSRL;

            break;

        case 0x20: // OR
        case 0x28: // ORI
            rs.op = EX_ALU_OR;
            break;

        case 0x21: // AND
        case 0x29: // ANDI
            rs.op = EX_ALU_AND;
            break;

        case 0x22: // XOR
        case 0x2A: // XORI
            rs.op = EX_ALU_XOR;
            break;

        case 0x23: // ANDN
        case 0x2B: // ANDNI
            rs.op = EX_ALU_ANDN;
            break;

        case 0x24:
            rs.Qk = ROB_TAG_INVALID;

            switch (instr.func) {
                case 0x001:
                    rs.op = EX_ALU_SRA;
                    write_carry = 1;
                    break;
                case 0x021:
                    rs.Ql = register_read(reg_gpr(REGISTER_FLAG_CARRY), &rs.Vl);
                    rs.op = EX_ALU_SRC;
                    write_carry = 1;
                    break;
                case 0x041:
                    rs.op = EX_ALU_SRL;
                    write_carry = 1;
                    break;
                case 0x060:
                    rs.op = EX_ALU_SEXT;
                    rs.Vk = 7;
                    break;
                case 0x061:
                    rs.op = EX_ALU_SEXT;
                    rs.Vk = 15;
                    break;

                default:
                    ABORT_WITH_MSG("unknown logical func: %03x", instr.func);
            }

            break;

        default:
            return 1;
    }

    if (execute_queue_alu(rs))
        return 1;

    if (write_carry) {
        rob_get_entry(tag)->write_carry = 1;
        register_write(reg_flag(REGISTER_FLAG_CARRY), tag);
    }

    register_write(reg_gpr(instr.rd), tag);

    return 0;
}

static int
dispatch_memory(rob_tag_t tag, word_t rawinstr)
{
    union mb_instr instr = { .raw = rawinstr };

    switch (instr.opcode) {
        case 0x30: // LBU
        case 0x38: // LBUI

        case 0x31: // LHU
        case 0x39: // LHUI

        case 0x32: // LW
        case 0x3A: // LWI
            break;

        case 0x34: // SB
        case 0x3C: // SBI

        case 0x35: // SH
        case 0x3D: // SHI

        case 0x36: // SW
        case 0x3E: // SWI
            break;

        default:
            return 1;
    }


    return 0;
}

static int
dispatch_branch(rob_tag_t tag, address_t pc, word_t rawinstr)
{
    union mb_instr instr = { .raw = rawinstr };
    struct rs_alu rs = { 0 };

    rs.busy = 1;
    rs.Qi = tag;

    if (instr.has_immediate)
        rs.Qk = extend_immediate(instr.imm16, &rs.Vk);
    else
        rs.Qk = register_read(reg_gpr(instr.rb), &rs.Vk);

    switch (instr.opcode) {
        case 0x26: // BR,  BRL,  BRA,  BRAL,  BRD,  BRLD,  BRAD,  BRALD
        case 0x2E: // BRI, BRLI, BRAI, BRALI, BRID, BRLID, BRAID, BRALID
            rob_get_entry(tag)->type = ROB_INSTR_BRANCH;
            rob_get_entry(tag)->br_taken = 1;
            rob_get_entry(tag)->br_delayed = instr.delayed_absolute;

            rs.Qj = ROB_TAG_INVALID;

            if (instr.absolute)
                rs.Vj = 0;
            else
                rs.Vj = pc;

            rs.Ql = ROB_TAG_INVALID;
            rs.Vl = pc;

            if (instr.delayed_absolute)
                rob_get_entry(tag)->type |= ROB_INSTR_DELAYED;

            rs.op = EX_ALU_BR;
            break;

        case 0x27: // BEQ,  BNE,  BLT,  BLE,  BGT,  BGE,  BEQD,  BNED,  BLTD,  BLED,  BGTD,  BGED
        case 0x2F: // BEQI, BNEI, BLTI, BLEI, BGTI, BGEI, BEQID, BNEID, BLTID, BLEID, BGTID, BGEID
            rob_get_entry(tag)->type = ROB_INSTR_CONDITIONAL | ROB_INSTR_BRANCH;
            rob_get_entry(tag)->br_delayed = instr.delayed_conditional;

            rs.Qj = register_read(reg_gpr(instr.ra), &rs.Vj);
            rs.Vl = pc;

            switch (instr.cond) {
                case 0:
                    rs.op = EX_ALU_BEQ;
                    break;
                case 1:
                    rs.op = EX_ALU_BNE;
                    break;
                case 2:
                    rs.op = EX_ALU_BLT;
                    break;
                case 3:
                    rs.op = EX_ALU_BLE;
                    break;
                case 4:
                    rs.op = EX_ALU_BGT;
                    break;
                case 5:
                    rs.op = EX_ALU_BGE;
                    break;
                default:
                    ABORT_WITH_MSG("invalid conditional branch code");
            }

            if (instr.delayed_conditional)
                rob_get_entry(tag)->type |= ROB_INSTR_DELAYED;

            break;

        default:
            return 1;
    }

    if (execute_queue_alu(rs))
        return 1;

    if (instr.link && (instr.opcode == 0x26 || instr.opcode == 0x2E))
        register_write(reg_gpr(instr.rd), tag);

    return 0;
}

static int
dispatch_other(rob_tag_t tag, word_t rawinstr)
{
    union mb_instr instr = { .raw = rawinstr };
    struct rs_alu rs = {0};

    rs.busy = 1;
    rs.Qi = tag;

    return 0;
}
