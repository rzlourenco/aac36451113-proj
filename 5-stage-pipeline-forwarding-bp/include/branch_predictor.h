#pragma once

#include "common.h"

void bp_branch_predict(word_t pc, int *taken, int *delayed, word_t *target);

void bp_branch_taken(word_t pc, word_t target, int delayed);
void bp_branch_not_taken(word_t pc, word_t target, int delayed);

void bp_init(void);
void bp_clock(void);
