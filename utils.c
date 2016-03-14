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
