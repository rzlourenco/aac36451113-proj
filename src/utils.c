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

// Source: https://graphics.stanford.edu/~seander/bithacks.html
int log2i(int value) {
    int const b[] = { 0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000 };
    int const S[] = { 1, 2, 4, 8, 16 };

    int result = 0;
    for (int i = 4; i >= 0; --i) {
        if (value & b[i]) {
            value >>= S[i];
            result |= S[i];
        }
    }

    return result;
}
