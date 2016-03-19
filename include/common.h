#pragma once

#include "utils.h"

#include <stdint.h>

typedef int16_t half_word_t;

typedef int32_t word_t;
typedef uint32_t uword_t;

typedef int64_t long_word_t;
typedef uint64_t long_uword_t;

typedef int32_t instruction_t;
typedef int32_t address_t;

#define STDOUT_MMIO ((address_t)0xFFFFFFC0)

#define WORD_MAX INT32_MAX
#define WORD_MIN INT32_MIN
