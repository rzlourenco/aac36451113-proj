#include "stdio.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    printf("Hello, world!\n");
    xil_printf("errno(%d) = %s\n", errno, strerror(errno));
    printf("Hello, world!\n");
    xil_printf("errno(%d) = %s\n", errno, strerror(errno));
    puts("Hello, world!\n");
    xil_printf("errno(%d) = %s\n", errno, strerror(errno));
    puts("Hello, world!\n");
    xil_printf("errno(%d) = %s\n", errno, strerror(errno));
    putchar('K');
    xil_printf("errno(%d) = %s\n", errno, strerror(errno));
    putchar('e');
    xil_printf("errno(%d) = %s\n", errno, strerror(errno));
    putchar('k');
    xil_printf("errno(%d) = %s\n", errno, strerror(errno));
    putchar('\n');
    xil_printf("errno(%d) = %s\n", errno, strerror(errno));
    fflush(stdout);
    xil_printf("errno(%d) = %s\n", errno, strerror(errno));

    write(1, "Hello, world!\n", 14);
    return 0;
}
