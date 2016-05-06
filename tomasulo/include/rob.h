#pragma once

#include "common.h"

NEWTYPE(rob_tag, word_t)

struct rob_entry {
    address_t pc;

    int type;
    int busy;
    int speculated;
    int keep_carry;
    int new_carry;

    int br_taken;
    int br_delayed;

    int br_pred_taken;
    word_t br_pred_target;

    word_t st_address;

    word_t value;
};

extern const rob_tag_t ROB_TAG_INVALID;

enum {
    ROB_INSTR_NORMAL = 0x00,
    ROB_INSTR_STORE = 0x01,
    ROB_INSTR_BRANCH = 0x02,
    ROB_INSTR_COND_BRANCH = 0x04 | ROB_INSTR_BRANCH,
};

void
rob_init(void);

int
rob_write(address_t pc, int br_pred, address_t target, rob_tag_t *tag);

struct rob_entry *
rob_get_entry(rob_tag_t tag);

void
rob_clock(void);
