#pragma once

#include "utils.h"
#include <stdint.h>

typedef int32_t word_t;
typedef uint32_t uword_t;
typedef int16_t hword_t;
typedef uint16_t uhword_t;

NEWTYPE(instruction, uword_t)
NEWTYPE(address, uword_t)
