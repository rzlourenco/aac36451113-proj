#include "stdio.h"
#include <stdio.h>

int main(void)
{
    printf("Hello, world!\n");
    printf("Hello, world!\n");
    puts("Hello, world!\n");
    puts("Hello, world!\n");
    putchar('K');
    putchar('e');
    putchar('k');
    putchar('\n');
    fflush(stdout);

    write(1, "Hello, world!\n", 14);
    return 0;
}
