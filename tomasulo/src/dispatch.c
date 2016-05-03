#include "dispatch.h"

#include "cpu.h"
#include "execute.h"
#include "issue.h"
#include "register.h"
#include "rob.h"

#include <assert.h>

enum {
    QUEUE_SIZE = ISSUE_WIDTH * 10,
};

static struct {
    rob_tag_t tag;
    word_t instr;
} queue[QUEUE_SIZE];

size_t queue_head = 0, queue_tail = 0, queue_size = 0;

int dispatch_queue_instruction(word_t tag, word_t instr) {
    if (queue_size == QUEUE_SIZE)
        return 1;

    queue[queue_tail].tag = tag;
    queue[queue_tail].instr = instr;
    queue_tail = (queue_tail + 1) % QUEUE_SIZE;
    queue_size += 1;

    return 0;
}

static word_t extend_immediate(sh_word_t immediate);

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

void dispatch_clock(void) {

    // add r1, r2, r3
    // addc r5, r0, r0
}
