#include "issue.h"

#include "dispatch.h"
#include "rob.h"

enum {
    BUFFER_SIZE = 2*ISSUE_WIDTH,
};

static struct fetched {
    address_t pc;
    word_t instr;

    int br_prediction;
    address_t br_target;

    int in_rob;
    rob_tag_t rob_tag;
}
queue[BUFFER_SIZE];

static size_t
queue_head = 0,
queue_tail = 0,
queue_count = 0;

int
issue_queue_instruction(address_t pc, word_t instr, int predicted, address_t target)
{
    if (queue_count == BUFFER_SIZE)
        return 1;

    queue[queue_tail] = (struct fetched){
        .pc = pc,
        .instr = instr,
        .br_prediction = predicted,
        .br_target = target,
        .in_rob = 0,
        .rob_tag = ROB_TAG_INVALID,
    };

    queue_tail = (queue_tail + 1) % BUFFER_SIZE;
    queue_count += 1;

    return 0;
}

static void
pop_instruction(void)
{
    assert(queue_count > 0);

    queue_head = (queue_head + 1) % BUFFER_SIZE;
    queue_count -= 1;
}

void
issue_clock(void)
{
    while (queue_count > 0) {
        struct fetched *head = &queue[queue_head];

        if (head->in_rob == 0) {
            if (rob_write(head->pc, head->br_prediction, head->br_target, &head->rob_tag))
                return;
        }

        head->in_rob = 1;

        if (dispatch_queue_instruction(head->rob_tag, head->instr))
            return;

        pop_instruction();
    }
}
