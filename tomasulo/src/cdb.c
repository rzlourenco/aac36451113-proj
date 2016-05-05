#include "cdb.h"

#include "issue.h"

#include "cpu.h"

static struct {
    rob_tag_t tag;
    word_t data;
} wdata[ISSUE_WIDTH], rdata[ISSUE_WIDTH];

static int wsize = 0, rsize = 0;

int cdb_write(rob_tag_t tag, word_t data) {
    if (wsize >= ISSUE_WIDTH) {
        cpu_stats.sc_cdb += 1;
        return 1;
    }

    wdata[wsize].tag = tag;
    wdata[wsize].data = data;

    return 0;
}

int cdb_read(rob_tag_t tag, word_t *data) {
    for (int i = 0; i < rsize; ++i) {
        if (rob_tag_eq(rdata[i].tag, tag)) {
            *data = rdata[i].data;
            return 0;
        }
    }

    return 1;
}

void cdb_clock(void) {
    rsize = wsize;
    wsize = 0;

    for (int i = 0; i < ISSUE_WIDTH; ++i) {
        rdata[i] = wdata[i];
    }
}
