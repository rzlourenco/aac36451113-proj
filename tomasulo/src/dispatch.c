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
    word_t instr;
}
wqueue[QUEUE_SIZE],
rqueue[QUEUE_SIZE];

static size_t
wqueue_head = 0,
wqueue_tail = 0,
wqueue_size = 0,
rqueue_head = 0,
rqueue_tail = 0,
rqueue_size = 0;

int
dispatch_queue_instruction(rob_tag_t tag, word_t instr)
{
    if (wqueue_size == QUEUE_SIZE) {
        cpu_stats.sc_dispatch += 1;
        return 1;
    }

    wqueue[wqueue_tail].tag = tag;
    wqueue[wqueue_tail].instr = instr;

    wqueue_size += 1;
    wqueue_tail += 1;
    if (wqueue_tail == QUEUE_SIZE)
        wqueue_tail = 0;

    return 0;
}

union mb_instr {
    struct {
        word_t opcode: 6;
        word_t rd: 5;
        word_t ra: 5;
        word_t rb: 5;
        word_t func: 11;
    };
    struct {
        // opcode
        word_t _0: 2;
        word_t i: 1;
        word_t k: 1;
        word_t c: 1;
        word_t _1: 1;

        // rd
        word_t dc: 1;
        word_t _2: 4;

        // ra
        word_t da: 1;
        word_t a: 1;
        word_t l: 1;
        word_t _3: 2;

        // rb
        word_t _4: 5;

        // func
        word_t s: 1;
        word_t t: 1;
        word_t _5: 7;
        word_t u: 1;
        word_t _6: 1;
    };
    struct {
        word_t _7: 16;
        word_t imm16: 16;
    };
    struct {
        word_t _8: 8;
        word_t cond: 3;
        word_t _9: 16;
        word_t imm5: 5;
    };
    word_t raw;
};

static void
copy_front(void)
{
    for (int i = 0; i < QUEUE_SIZE; ++i)
        rqueue[i] = wqueue[i];

    rqueue_head = wqueue_head;
    rqueue_tail = wqueue_tail;
    rqueue_size = wqueue_size;
}

static void
copy_back(void)
{
    for (int i = 0; i < QUEUE_SIZE; ++i)
        wqueue[i] = rqueue[i];

    wqueue_head = rqueue_head;
    wqueue_tail = rqueue_tail;
    wqueue_size = rqueue_size;
}

static struct taginstr *top(void);

static void pop(void);

static int dispatch_alu(rob_tag_t tag, word_t rawinstr);

void
dispatch_clock(void)
{
    copy_front();

    for (int i = 0; i < ISSUE_WIDTH; ++i) {
        if (rqueue_size == 0)
            break;

        if (!dispatch_alu(top()->tag, top()->instr))
            pop();
    }

    copy_back();
}

static struct taginstr *
top(void)
{
    assert(rqueue_size > 0);

    return &rqueue[rqueue_head];
}

static void
pop(void)
{
    assert(rqueue_size > 0);

    rqueue_head += 1;
    if (rqueue_head == QUEUE_SIZE)
        rqueue_head = 0;
}

static rob_tag_t
extend_immediate(word_t immediate, word_t *data)
{
    return ROB_TAG_INVALID;
}

static int
dispatch_alu(rob_tag_t tag, word_t rawinstr)
{
    union mb_instr instr = { .raw = REVERSE_BYTES_32(rawinstr) };
    struct rs_alu rs = {0};

    rs.busy = 1;
    rs.Qj = register_read(reg_gpr(instr.ra), &rs.Vj);

    if (instr.i)
        rs.Qk = extend_immediate(instr.imm16, &rs.Vk);
    else
        rs.Qk = register_read(reg_gpr(instr.rb), &rs.Vk);

    if (instr.c)
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

    if (!instr.k && instr.opcode != 0x10 && instr.opcode != 0x18)
        register_write(reg_flag(REGISTER_FLAG_CARRY), tag);

    register_write(reg_gpr(instr.rd), tag);
}
