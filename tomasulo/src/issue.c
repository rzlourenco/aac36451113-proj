#include "issue.h"

static struct {
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

void issue_clock(void) {

}
