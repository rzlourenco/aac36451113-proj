#pragma once

#include "rob.h"

int cdb_write(rob_tag_t tag, word_t data);
int cdb_read(rob_tag_t tag, word_t *data);

void cdb_clock(void);
