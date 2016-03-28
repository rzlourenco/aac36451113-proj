#include "stdio.h"
#include <stdio.h>

int fib(int i) {
    int prev, curr;
    prev = 0;
    curr = 1;

    while (i-- > 0) {
        int tmp = prev;
        prev = curr;
        curr += tmp;
    }

    return curr;
}

int main(void) {
    int i;

    for (i = 0; i < 10; ++i) {
        xil_printf("fib(%d) = %d\n", i, fib(i));
    }
}
