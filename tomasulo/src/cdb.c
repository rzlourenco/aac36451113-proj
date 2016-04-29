#include "cdb.h"

#include "issue.h"

#include "cpu.h"

static struct {
    word_t tag;
    word_t data;
} cdbwdata[ISSUE_WIDTH], cdbrdata[ISSUE_WIDTH];

static int filled_w = 0, filled_r = 0;

int cdb_write(word_t tag, word_t data) {
    if (filled_w >= ISSUE_WIDTH) {
        cpu_state.stats.cdb_stalls++;
        return 1;
    }

    cdbwdata[filled_w].tag = tag;
    cdbwdata[filled_w].data = data;

    return 0;
}

int cdb_read(word_t tag, word_t *data) {
    for (int i = 0; i < filled_r; ++i) {
        if (cdbrdata[i].tag == tag) {
            *data = cdbrdata[i].data;
            return 0;
        }
    }

    return 1;
}

void cdb_clock(void) {
    filled_r = filled_w;
    filled_w = 0;

    for (int i = 0; i < ISSUE_WIDTH; ++i) {
        cdbrdata[i] = cdbwdata[i];
    }
}
