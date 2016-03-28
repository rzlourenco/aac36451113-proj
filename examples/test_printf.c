#include "stdio.h"

int log2i(int a) {
    int ret = 0;

    while (a > 1) {
        ret++;
        a >>= 1;
    }

    return ret;
}

int main() {
    int a, b, c;

    a = 31415;
    b = a * a;
    c = log2i(b);

    xil_printf("a = %d, b = %d, c = %d\n", a, b, c);

    return 0;
}
