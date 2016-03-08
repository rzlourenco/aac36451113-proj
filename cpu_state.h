#pragma once

#include "common.h"

struct cpu_state {
    address_t pc;
};

extern struct cpu_state cpu_state;
