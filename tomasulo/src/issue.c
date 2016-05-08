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
queue[QUEUE_SIZE],
tmp[QUEUE_SIZE];

static size_t
queue_head = 0,
queue_tail = 0,
queue_size = 0,
tmp_head = 0,
tmp_tail = 0,
tmp_size = 0;

int
issue_queue_instruction(address_t pc, word_t instr, int taken, int delayed, address_t target)
{
    if (tmp_size + queue_size == QUEUE_SIZE) {
        cpu_stats.sc_issue += 1;
        return 1;
    }

    tmp[tmp_tail] = (struct fetched){
        .pc = pc,
        .instr = instr,
        .br_taken = taken,
        .br_delayed = delayed,
        .br_target = target,
        .in_rob = 0,
        .rob_tag = ROB_TAG_INVALID,
    };

    tmp_tail = (tmp_tail + 1) % QUEUE_SIZE;
    tmp_size += 1;

    return 0;
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

void
issue_clock(void)
{
    for (int i = 0; i < ISSUE_WIDTH; ++i) {
        if (queue_size == 0)
            break;

        struct fetched *head = &queue[queue_head];

        if (head->in_rob == 0) {
            if (rob_write(head->pc, head->br_taken, head->br_target, &head->rob_tag))
                break;
        }

        head->in_rob = 1;

        if (dispatch_queue_instruction(head->rob_tag, head->pc, head->instr))
            break;

        pop_instruction();
    }

    while (tmp_size > 0)
        push_instruction();

    tmp_head = 0;
    tmp_tail = 0;
}

void
issue_dump(void)
{
    fprintf(stderr, "[issue] th=%u tt=%u tz=%u\n",
            tmp_head,
            tmp_tail,
            tmp_size
    );

    for (int i = 0; i < tmp_size; ++i) {
        size_t ix = (tmp_head + i) % QUEUE_SIZE;
        fprintf(stderr, "[issue]     T %d@%zu pc:%08x t:%d d:%d T:%08x r:%d R:%u\n",
                i,
                ix,
                tmp[ix].pc,
                tmp[ix].br_taken,
                tmp[ix].br_delayed,
                tmp[ix].br_target,
                tmp[ix].in_rob,
                rob_tag_val(tmp[ix].rob_tag)
        );
    }

    fprintf(stderr, "[issue] qh=%u qt=%u qz=%u\n",
            queue_head,
            queue_tail,
            queue_size
    );

    for (int i = 0; i < queue_size; ++i) {
        size_t ix = (queue_head + i) % QUEUE_SIZE;
        fprintf(stderr, "[issue]     Q %d@%zu pc:%08x t:%d d:%d T:%08x r:%d R:%u\n",
                i,
                ix,
                queue[ix].pc,
                queue[ix].br_taken,
                queue[ix].br_delayed,
                queue[ix].br_target,
                queue[ix].in_rob,
                rob_tag_val(queue[ix].rob_tag)
        );
    }

}
