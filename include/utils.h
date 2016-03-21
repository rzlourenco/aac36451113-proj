#pragma once

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define BITS(VALUE, LOW, HIGH) (((VALUE)>>(LOW))&((1u<<((HIGH)-(LOW)+1u))-1u))

// Source: https://graphics.stanford.edu/~seander/bithacks.html
#define SIGN_EXTEND(VALUE, BIT_COUNT) ((((VALUE)&((1u<<(BIT_COUNT))-1u))^(1u<<((BIT_COUNT)-1u)))-((1U<<((BIT_COUNT)-1u))))

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

#define STRINGIFY(ARG) #ARG
#define TOSTRING(ARG) STRINGIFY(ARG)

#define ABORT_MSG_HELPER(MSG, ...) do { \
    fprintf(stderr, __FILE__ ":" TOSTRING(__LINE__) ": " MSG "\n%s", __VA_ARGS__);\
    abort();\
} while (0)

#define ABORT_MSG(...) ABORT_MSG_HELPER(__VA_ARGS__, "")

int log2i(int value);
