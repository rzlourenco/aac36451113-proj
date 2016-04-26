#include "issue.h"

#include <assert.h>

static struct fetched_instr {
    address_t pc;
    word_t instr;

    int predicted;
    address_t target;
} fetched[ISSUE_WIDTH];

static int fetch_count = 0;

int issue_fetch(address_t pc, word_t instr, int predicted, address_t target) {
    if (fetch_count >= ISSUE_WIDTH)
        return 1;

    fetched[fetch_count].pc = pc;
    fetched[fetch_count].instr = instr;
    fetched[fetch_count].predicted = predicted;
    fetched[fetch_count].target = target;

    fetch_count++;
    return 0;
}

int has_instruction(void) {
    return fetch_count > 0;
}

struct fetched_instr get_instruction(void) {
    assert(has_instruction());

    return fetched[0];
}

void pop_instruction(void) {
    for (int i = 1; i < ISSUE_WIDTH; ++i) {
        fetched[i - 1] = fetched[i];
        fetched[i] = (struct fetched_instr) {0};
    }

    fetch_count--;
}

union mb_instr {
    struct {
        union {
            struct {
                word_t: 2;
                word_t i: 1;
                word_t c: 1;
                word_t k: 1;
                word_t: 1;
            };
            struct {
                word_t opcode : 6;
            };
        };
        union {
            struct {
                word_t rd: 5;
            };
            struct {
                word_t d: 1;
                word_t: 1;
                word_t cond: 3;
            };
        };
        word_t ra : 5;
        union {
            struct {
                word_t rb: 5;
                union {
                    struct {
                        word_t func: 11;
                    };
                    struct {
                        word_t s: 1;
                        word_t t: 1;
                        word_t: 4;
                        s_word_t imm5: 5;
                    };
                };
            };
            struct {
                s_word_t imm: 16;
            };
        };
    };
    word_t raw;
};

int issue_arithmetical(struct fetched_instr);

int issue_logical(struct fetched_instr);

int issue_branch(struct fetched_instr);

int issue_memory(struct fetched_instr);

int issue_other(struct fetched_instr);

void issue_clock(void) {
    while (has_instruction()) {
        struct fetched_instr fetched = get_instruction();

        if (!issue_arithmetical(fetched))
            pop_instruction();
        else if (!issue_logical(fetched))
            pop_instruction();
        else if (!issue_branch(fetched))
            pop_instruction();
        else if (!issue_memory(fetched))
            pop_instruction();
        else if (!issue_other(fetched))
            pop_instruction();
        else {
            ABORT_WITH_MSG(
                    "illegal instruction pc:%08x instr:%08x",
                    fetched.pc,
                    fetched.instr
            );
        }
    }
}

int issue_arithmetical(struct fetched_instr fetched) {
    union mb_instr instr;
    instr.raw = fetched.instr;
}
