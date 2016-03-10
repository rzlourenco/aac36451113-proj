#include "cpu_state.h"
#include "memory.h"

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int opt, mem_bits = 16, rom_fd = -1, rom_errno;

    while ((opt = getopt(argc, argv, "b:r:")) != -1) {
        switch (opt) {
            case 'b':
                mem_bits = atoi(optarg);
                break;
            case 'r':
                rom_fd = open(optarg, O_RDONLY);
                rom_errno = errno;
                break;
            default:
                fprintf(stderr, "Usage: %s -b address_bits -r rom\n", argv[0]);
                return 1;
        }
    }

    init_cpu();


    while (!cpu_state.halt) {
        clock();
    }

    return 0;
}
