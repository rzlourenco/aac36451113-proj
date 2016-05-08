#include "commit.h"

#include "branch_predictor.h"
#include "cdb.h"
#include "cpu.h"
#include "issue.h"
#include "memory.h"
#include "register.h"
#include "rob.h"

enum {
    // Value chosen by prayer
    QUEUE_SIZE = ISSUE_WIDTH,
};

const rob_tag_t ROB_TAG_INVALID = {0};

static struct rob_entry
queue[QUEUE_SIZE], tmp[QUEUE_SIZE];

static size_t
tmp_head = 0,
tmp_tail = 0,
tmp_size = 0,
queue_head = 0,
queue_tail = 0,
queue_size = 0;

int
rob_write(address_t pc, int br_pred, address_t target, rob_tag_t *tag)
{
    if (tmp_size + queue_size == QUEUE_SIZE) {
        cpu_stats.sc_rob += 1;
        return 1;
    }

    tmp[tmp_tail] = (struct rob_entry){0};
    tmp[tmp_tail] = (struct rob_entry){
            .pc = pc,

            .busy = 1,

            .br_pred_taken = br_pred,
            .br_pred_target = target,

            .type = ROB_INSTR_NORMAL,
    };

    *tag = make_rob_tag(tmp_tail + 1);
    tmp_tail = (tmp_tail + 1) % QUEUE_SIZE;
    tmp_size += 1;

    return 0;
}

struct rob_entry *
rob_get_entry(rob_tag_t tag_)
{
    word_t tag = rob_tag_val(tag_);

    if (rob_tag_eq(tag_, ROB_TAG_INVALID) || tag > QUEUE_SIZE || queue_size == 0)
        return NULL;

    tag -= 1;

    size_t head = queue_head;
    size_t tail = queue_tail;
    size_t index = tag;

    if (tail <= head) {
        tail += QUEUE_SIZE;
    }

    if (index < head || index >= tail)
        return NULL;

    return &queue[tag];
}

static void
push_instruction(void)
{
    assert(tmp_size > 0);
    assert(queue_size < QUEUE_SIZE);

    queue[queue_tail] = tmp[tmp_head];
    tmp[tmp_head] = (struct rob_entry){0};

    tmp_head = (tmp_head + 1) % QUEUE_SIZE;
    tmp_size -= 1;

    queue_tail = (queue_tail + 1) % QUEUE_SIZE;
    queue_size += 1;
}

void
rob_clock(void)
{
    for (size_t i = 0; i < queue_size; ++i) {
        if (queue[i].busy == 0)
            continue;

        size_t index = (queue_head + i) % QUEUE_SIZE;
        rob_tag_t tag = make_rob_tag(index + 1);

        cdb_read(tag, &queue[index].value);
    }

    while (tmp_size > 0)
        push_instruction();

    tmp_head = 0;
    tmp_tail = 0;
}

static void
pop_instruction(void)
{
    assert(queue_size > 0);

    queue[queue_head] = (struct rob_entry){0};
    queue_head = (queue_head + 1) % QUEUE_SIZE;
    queue_size -= 1;
}

void
commit_clock(void)
{
    for (int i = 0; i < ISSUE_WIDTH; ++i) {
        if (queue_size == 0) {
            cpu_stats.sc_commit += 1;
            cpu_stats.sc_commit_empty += 1;
            break;
        }

        struct rob_entry *head = &queue[queue_head];

        if (head->busy) {
            cpu_stats.sc_commit += 1;
            cpu_stats.sc_commit_stall += 1;
            break;
        }

        if (head->write_carry)
            register_real_write(reg_flag(REGISTER_FLAG_CARRY), head->new_carry != 0u);

        if (head->write_register)
            register_real_write(head->dest_register, head->value);

        if (head->type == ROB_INSTR_STORE)
            memory_write(head->st_address, head->value);

        if (head->type & ROB_INSTR_BRANCH) {
            if (head->type & ROB_INSTR_CONDITIONAL)
                head->br_taken = head->value != 0;

            if (head->br_taken)
                bp_branch_taken(head->pc, head->value, head->br_delayed);
            else
                bp_branch_not_taken(head->pc, head->value, head->br_delayed);

            // TODO FIXME
            cpu_branch(head->value);
        }

        pop_instruction();
        cpu_stats.instructions += 1;
    }
}
