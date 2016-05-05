#pragma once

#include "rob.h"

#include "common.h"

int dispatch_queue_instruction(rob_tag_t tag, word_t instr);

void dispatch_clock(void);
