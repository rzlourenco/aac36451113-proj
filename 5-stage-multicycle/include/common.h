#pragma once

#include "utils.h"

#include <stdint.h>

typedef uint16_t h_word_t;
typedef int16_t sh_word_t;

typedef uint32_t word_t;
typedef int32_t s_word_t;

typedef uint64_t l_word_t;
typedef int64_t sl_word_t;

typedef word_t instruction_t;
typedef word_t address_t;

#define STDOUT_MMIO ((address_t)0xFFFFFFC0)

#define WORD_MAX INT32_MAX
#define WORD_MIN INT32_MIN
