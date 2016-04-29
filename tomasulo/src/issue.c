#include "issue.h"

#include "dispatch.h"
#include "rob.h"

static struct fetched_instr {
    address_t pc;
    word_t instr;

    int br_prediction;
    int br_taken;
    address_t br_target;
} fetched[ISSUE_WIDTH];

static int fetch_count = 0;

int issue_queue_instruction(address_t pc, word_t instr, int predicted, address_t target) {
    if (fetch_count >= ISSUE_WIDTH)
        return 1;

    fetched[fetch_count].pc = pc;
    fetched[fetch_count].instr = instr;

    fetched[fetch_count].br_prediction = predicted;
    fetched[fetch_count].br_taken = 0;
    fetched[fetch_count].br_target = target;

    fetch_count++;
    return 0;
}

static int has_instruction(void) {
    return fetch_count > 0;
}

static struct fetched_instr get_instruction(void) {
    assert(has_instruction());

    return fetched[0];
}

static void pop_instruction(void) {
    for (int i = 1; i < ISSUE_WIDTH; ++i) {
        fetched[i - 1] = fetched[i];
        fetched[i] = (struct fetched_instr) {0};
    }

    fetch_count--;
}

void issue_clock(void) {
    while (has_instruction()) {
        struct fetched_instr fetched = get_instruction();
    }
}
