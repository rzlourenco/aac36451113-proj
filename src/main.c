#include "cpu_state.h"
#include "memory.h"

#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>

void usage(char const *argv0) {
    fprintf(stderr, "Usage: %s [ -b address_bits ] -r rom_file\n", argv0);
    exit(1);
}

int main(int argc, char **argv) {
    assert(sizeof(struct msr_t) == sizeof(word_t));

    struct sigaction action = { .sa_handler = cpu_dump, .sa_flags = 0 };
    assert(sigaction(SIGABRT, &action, NULL) != -1);
    assert(sigaction(SIGINT, &action, NULL) != -1);

    int opt;
    size_t mem_bits = 16;
    char *rom_file = NULL;

    while ((opt = getopt(argc, argv, "b:r:")) != -1) {
        switch (opt) {
            case 'b':
                mem_bits = (size_t)atoi(optarg);
                break;
            case 'r':
                rom_file = strdup(optarg);
                break;
            default:
                usage(argv[0]);
        }
    }

    if (rom_file == NULL) {
        usage(argv[0]);
    }

    if (mem_bits < 10 || mem_bits > 32) {
        fprintf(stderr, "Invalid address bits. Valid range: [10, 32]\n");
        exit(2);
    }

    int rom_fd = open(rom_file, O_RDONLY);
    if (rom_fd < 0) {
        perror("open");
        exit(2);
    }

    free(rom_file);

    struct stat rom_stat;
    if (fstat(rom_fd, &rom_stat) < 0) {
        perror("stat");
        exit(2);
    }

    assert(rom_stat.st_size > 0);
    void *rom = mmap(NULL, (size_t)rom_stat.st_size, PROT_READ, MAP_PRIVATE, rom_fd, 0);
    if (rom == MAP_FAILED) {
        perror("mmap");
        exit(2);
    }

    if (0) {
        if (rom_stat.st_size % sizeof(word_t) != 0) {
            fprintf(stderr, "Invalid ROM size (%zd): not a multiple of %zd bytes\n", (size_t)rom_stat.st_size, sizeof(word_t));
            exit(2);
        }
    }

    init_cpu();
    init_memory(mem_bits);
    flash_memory(rom, (size_t)rom_stat.st_size);

    if (munmap(rom, (size_t)rom_stat.st_size) < 0) {
        perror("munmap");
        exit(2);
    }

    cpu_dump(0);

    while (!cpu_halt()) {
        clock();
        cpu_dump(0);
    }

    return 0;
}
