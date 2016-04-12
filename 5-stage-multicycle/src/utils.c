#include "utils.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifndef _POSIX_VERSION

char *strdup(char const *s) {
    size_t len = strlen(s) + 1;
    char *ret = malloc(len);

    if (ret == NULL) {
        return NULL;
    }

    memcpy(ret, s, len);
    return ret;
}

#endif

uint64_t sign_extend(uint64_t val, uint64_t bits, uint64_t sign_bit) {
    assert(bits <= 64);
    assert(sign_bit < bits);

    uint64_t value_mask = ~(uint64_t)0 >> (64 - bits);
    val &= value_mask;

    uint64_t sign_mask = (~(uint64_t)0 >> sign_bit) << sign_bit;
    val |= (val & sign_mask) ? sign_mask : 0;
    val &= value_mask;

    return val;
}
