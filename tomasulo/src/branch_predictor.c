#include "branch_predictor.h"

enum {
    BTB_PRED_BITS = 2,
    BTB_PRED_SAT_MAX = (1<<BTB_PRED_BITS)-1,
    BTB_PRED_SAT_MIN = 0,
    BTB_PRED_TAKEN = 1 + (BTB_PRED_SAT_MAX>>1),

    BTB_INDEX_BITS = 12,
    BTB_SIZE = 1<<BTB_INDEX_BITS,

    BTB_TAG_BITS = 8*sizeof(word_t) - BTB_INDEX_BITS,
};

static struct {
    word_t tag : BTB_TAG_BITS;
    word_t bits : BTB_PRED_BITS;
    word_t delayed : 1;
    word_t target;
} btb[BTB_SIZE];

static word_t get_index(word_t pc);
static word_t get_tag(word_t pc);

void bp_branch_predict(word_t pc, int *taken, int *delayed, word_t *target) {
    word_t index = get_index(pc);
    word_t tag = get_tag(pc);

    if (btb[index].tag != tag) {
        *taken = 0;
        return;
    }

    *taken = btb[index].bits >= BTB_PRED_TAKEN;
    *delayed = btb[index].delayed;
    *target = btb[index].target;
}

void bp_branch_taken(word_t pc, word_t target, int delayed) {
    word_t index = get_index(pc);
    word_t tag = get_tag(pc);

    btb[index].tag = tag;
    btb[index].target = target;
    btb[index].delayed = (word_t)(delayed != 0);

    if (btb[index].bits < BTB_PRED_SAT_MAX)
        btb[index].bits += 1;
}

void bp_branch_not_taken(word_t pc, word_t target, int delayed) {
    word_t index = get_index(pc);
    word_t tag = get_tag(pc);

    btb[index].tag = tag;
    btb[index].target = target;
    btb[index].delayed = (word_t)(delayed != 0);

    if (btb[index].bits > BTB_PRED_SAT_MIN)
        btb[index].bits -= 1;
}

void bp_init(void) {
    for (int i = 0; i < BTB_SIZE; ++i) {
        btb[i].tag = 0;
        btb[i].target = 0;
        btb[i].bits = ~0u;
        btb[i].bits >>= 1;
    }
}

void bp_clock(void) {

}

static word_t get_index(word_t pc) {
    return BITS(pc, 0, BTB_INDEX_BITS-1);
}

static word_t get_tag(word_t pc) {
    return pc >> BTB_INDEX_BITS;
}
