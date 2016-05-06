#include "rob.h"

#include "cpu.h"
#include "issue.h"

enum {
    // Value chosen by prayer
    QUEUE_SIZE = ISSUE_WIDTH,
};

const rob_tag_t ROB_TAG_INVALID = {0};

static struct rob_entry
rqueue[QUEUE_SIZE], wqueue[QUEUE_SIZE];

static size_t
wqueue_head = 0,
wqueue_tail = 0,
wqueue_size = 0,
rqueue_head = 0,
rqueue_tail = 0,
rqueue_size = 0;

int
rob_write(address_t pc, int br_pred, address_t target, rob_tag_t *tag)
{
    if (wqueue_size == QUEUE_SIZE) {
        cpu_stats.sc_rob += 1;
        return 1;
    }

    wqueue[wqueue_tail] = (struct rob_entry){0};
    wqueue[wqueue_tail] = (struct rob_entry){
            .pc = pc,

            .br_pred_taken = br_pred,
            .br_pred_target = target,

            .type = ROB_INSTR_NORMAL,
    };

    *tag = make_rob_tag(wqueue_tail + 1);
    wqueue_tail = (wqueue_tail + 1) % QUEUE_SIZE;
    wqueue_size += 1;

    return 0;
}

struct rob_entry *
rob_get_entry(rob_tag_t tag_)
{
    word_t tag = rob_tag_val(tag_);

    if (rob_tag_eq(tag_, ROB_TAG_INVALID) || tag > QUEUE_SIZE)
        return NULL;

    tag -= 1;

    size_t head = wqueue_head;
    size_t tail = wqueue_tail;
    size_t index = tag;

    if (tail < head) {
        tail += QUEUE_SIZE;
        index += QUEUE_SIZE;
    }

    if (index < head || index >= tail)
        return NULL;

    return &wqueue[tag];
}

static void copy_front(void) {
    for (int i = 0; i < QUEUE_SIZE; ++i)
        rqueue[i] = wqueue[i];

    rqueue_size = wqueue_size;
    rqueue_head = wqueue_head;
    rqueue_tail = wqueue_tail;
}

static void copy_back(void) {
    for (int i = 0; i < QUEUE_SIZE; ++i)
        wqueue[i] = rqueue[i];

    wqueue_size = rqueue_size;
    wqueue_head = rqueue_head;
    wqueue_tail = rqueue_tail;
}

void
rob_clock(void)
{
    copy_front();

    for (int i = 0; i < rqueue_size; ++i) {

    }

    copy_back();
}
