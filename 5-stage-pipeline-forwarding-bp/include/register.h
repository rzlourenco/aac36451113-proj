#pragma once

#include "common.h"

// FIXME: horrible naming...
struct register_status_t {
    int in_ex;
    int is_ex_load;
    int in_mem;
    int is_mem_load;
    int in_wb;
    int is_wb_load;
};

word_t register_read(address_t reg);
void register_write(address_t reg, word_t data);

void register_dump(void);

void register_clock(void);

struct register_status_t register_status(address_t reg);

void register_mark_load(address_t reg);
