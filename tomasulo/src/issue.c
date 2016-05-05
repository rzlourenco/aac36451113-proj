#include "issue.h"

#include "cpu.h"
#include "dispatch.h"
#include "rob.h"

enum {
    QUEUE_SIZE = ISSUE_WIDTH,
};

static struct fetched {
    address_t pc;
    word_t instr;

    int br_taken;
    int br_delayed;
    address_t br_target;

    int in_rob;
    rob_tag_t rob_tag;
}
rqueue[QUEUE_SIZE],
wqueue[QUEUE_SIZE];

static size_t
rqueue_head = 0,
rqueue_tail = 0,
rqueue_size = 0,
wqueue_head = 0,
wqueue_tail = 0,
wqueue_size = 0;

int
issue_queue_instruction(address_t pc, word_t instr, int taken, int delayed, address_t target)
{
    if (wqueue_size == QUEUE_SIZE) {
        cpu_stats.sc_issue += 1;
        return 1;
    }

    wqueue[wqueue_tail] = (struct fetched){
        .pc = pc,
        .instr = instr,
        .br_taken = taken,
        .br_delayed = delayed,
        .br_target = target,
        .in_rob = 0,
        .rob_tag = ROB_TAG_INVALID,
    };

    wqueue_tail = (wqueue_tail + 1) % QUEUE_SIZE;
    wqueue_size += 1;

    return 0;
}

static void
pop_instruction(void)
{
    assert(wqueue_size > 0);

    wqueue_head = (wqueue_head + 1) % QUEUE_SIZE;
    wqueue_size -= 1;
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
issue_clock(void)
{
    copy_front();

    while (rqueue_size > 0) {
        struct fetched *head = &rqueue[rqueue_head];

        if (head->in_rob == 0) {
            if (rob_write(head->pc, head->br_taken, head->br_target, &head->rob_tag))
                break;
        }

        head->in_rob = 1;

        if (dispatch_queue_instruction(head->rob_tag, head->instr))
            break;

        pop_instruction();
    }

    copy_back();
}
