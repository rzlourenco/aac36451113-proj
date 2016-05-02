#include <issue.h>
#include "rob.h"

enum {
    // Value chosen by prayer
    ROB_SIZE = 7 * ISSUE_WIDTH,
};

static struct rob_entry
rqueue[ROB_SIZE], wqueue[ROB_SIZE];

static size_t
rqueue_head = 0,
rqueue_tail = 0,
rqueue_size = 0,
wqueue_head = 0,
wqueue_tail = 0,
wqueue_size = 0;

int
rob_write(address_t pc, int br_pred, address_t target, rob_tag_t *tag)
{
    if (wqueue_size == ROB_SIZE)
        return 1;

    wqueue[wqueue_tail] = (struct rob_entry){
        .pc = pc,
        .br_pred_taken = br_pred,
        .br_pred_target = target,
        .type = ROB_INSTR_NORMAL,
    };

    *tag = (rob_tag_t)(wqueue_tail + 1);
    wqueue_tail = (wqueue_tail + 1) % ROB_SIZE;
    wqueue_size += 1;

    return 0;
}

void rob_clock(void) {

}
