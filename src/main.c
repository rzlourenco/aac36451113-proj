#include "cpu_state.h"
#include "memory.h"

#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void usage(char const *argv0) {
    fprintf(stderr, "Usage: %s [ -b address_bits ] [ -e (b|l) ] -r rom_file\n", argv0);
    fprintf(stderr, "\n");
    fprintf(stderr, " -e (b|l)\tAssume big (b) or little (l) endianness. Little endian by default.\n");
    exit(1);
}

int main(int argc, char **argv) {
    assert(sizeof(struct msr_t) == sizeof(word_t));

    int opt;
    size_t mem_bits = 16;
    char *rom_file = NULL;
    little_endian = 1;

    while ((opt = getopt(argc, argv, "b:e:r:")) != -1) {
        switch (opt) {
            case 'b':
                mem_bits = (size_t)atoi(optarg);
                break;
            case 'e':
                switch (*optarg) {
                    case 'b':
                    case 'B':
                        little_endian = 0;
                        break;
                    case 'l':
                    case 'L':
                        little_endian = 1;
                        break;
                    default:
                        fprintf(stderr, "Invalid endianness\n");
                        exit(2);
                        break;
                }
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

    struct sigaction action = { .sa_handler = cpu_dump, .sa_flags = 0 };
    if (sigaction(SIGABRT, &action, NULL) < 0 || sigaction(SIGINT, &action, NULL) < 0) {
        perror("sigaction");
        exit(2);
    }

    while (!cpu_halt()) {
        cpu_dump(0);
        clock();
        // (void)getchar();
    }

    cpu_dump(0);

    return 0;
}
