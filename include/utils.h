#pragma once

#define BITS(VALUE, LOW, HIGH) (((VALUE)>>(LOW))&((1u<<((HIGH)-(LOW)+1u))-1u))
#include <stdio.h>

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
}

#endif

#ifndef _POSIX_VERSION

char *strdup(char const *s);

#endif

#ifdef DEBUG
#define dprintf(...) fprintf(stderr, __VA_ARGS__)
#else
#define dprintf(...)
#endif

int log2i(int value);
