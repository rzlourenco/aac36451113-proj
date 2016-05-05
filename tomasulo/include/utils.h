#pragma once

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define BITS(VALUE, LOW, HIGH) (((VALUE)>>(LOW))&((1u<<((HIGH)-(LOW)+1u))-1u))

#define ARRAY_LENGTH(ARRAY) (sizeof(ARRAY)/sizeof((ARRAY)[0]))

#ifdef NDEBUG

#define NEWTYPE(TAG, REPR) \
\
typedef REPR TAG##_t;\
\
static inline const TAG##_t make_##TAG(REPR v) {\
    return v;\
}\
\
static inline REPR TAG##_val(TAG##_t v) {\
    return v;\
}

#else

#define NEWTYPE(TAG, REPR) \
\
typedef struct { REPR val; } TAG##_t;\
\
static inline TAG##_t make_##TAG(REPR v) {\
    return (TAG##_t){ .val = v };\
}\
\
static inline REPR TAG##_val(TAG##_t v) {\
    return v.val;\
}\
static inline int TAG##_eq(TAG##_t a, TAG##_t b) {\
    return a.val == b.val;\
}\

#endif

#ifndef _POSIX_VERSION

char *strdup(char const *s);

#endif

#ifdef DEBUG
#define dprintf(...) fprintf(stderr, __VA_ARGS__)
#else
#define dprintf(...)
#endif

#define STRINGIFY(ARG) #ARG
#define TOSTRING(ARG) STRINGIFY(ARG)

#define ABORT_MSG_HELPER(MSG, ...) do { \
    fprintf(stderr, __FILE__ ":" TOSTRING(__LINE__) ": " MSG "\n%s", __VA_ARGS__);\
    abort();\
} while (0)

#define ABORT_WITH_MSG(...) ABORT_MSG_HELPER(__VA_ARGS__, "")

#define REVERSE_BYTES_32(VAL) (0\
    |((((uint32_t)(VAL) >>  0) & 0xFF) << 24)\
    |((((uint32_t)(VAL) >>  8) & 0xFF) << 16)\
    |((((uint32_t)(VAL) >> 16) & 0xFF) <<  8)\
    |((((uint32_t)(VAL) >> 24) & 0xFF) <<  0)\
)

uint64_t sign_extend(uint64_t val, uint64_t bits, uint64_t sign_bit);
