#include "cpu_state.h"
#include "memory.h"

#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>


int little_endian;

int debug;

int has_breakpoint;
address_t breakpoint;

address_t trace_register;

int has_mem_trace;
address_t mem_trace;

FILE *trace_functions;


void usage(char const *argv0) {
    fprintf(stderr, "Usage: %s [options] rom_file\n", argv0);
    fprintf(stderr,
        "Options:\n"
        " -a <membits>  Address size in bits.\n"
        "               Defaults to 20 bits.\n"
        " -b <address>  Breakpoint. Enter debug mode when PC = <address>.\n"
        " -d            Enter debug mode.\n"
        " -e {b,l}      Assume big (b) or little (l) endianness.\n"
        "               Defaults to little endian.\n"
        " -f            Trace function calls (br[a][i]l[d] + rtsd).\n"
        " -m <address>  Trace writes to address <address>.\n"
        " -r <reg>      Trace writes to register <reg>.\n"
    );
    exit(1);
}

int main(int argc, char **argv) {
    assert(sizeof(struct msr_t) == sizeof(word_t));

    int opt;
    size_t mem_bits = 20;
    char *rom_file = NULL;

    little_endian = 1;

    debug = 0;

    has_breakpoint = 0;
    breakpoint = 0;

    has_mem_trace = 0;
    mem_trace = 0;

    trace_register = 0;

    trace_functions = NULL;

    while ((opt = getopt(argc, argv, "+b:de:fm:r:a:")) != -1) {
        switch (opt) {
            case 'a':
                mem_bits = (size_t)strtoul(optarg, NULL, 10);
                break;
            case 'b':
                has_breakpoint = 1;
                breakpoint = (word_t)strtoull(optarg, NULL, 16);
                break;
            case 'd':
                debug = 1;
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
            case 'f':
                trace_functions = fopen("call_stack", "w");
                assert(trace_functions);
                break;
            case 'm':
                has_mem_trace = 1;
                mem_trace = (word_t)strtoul(optarg, NULL, 16);
                break;
            case 'r':
                trace_register = (address_t)strtoul(optarg, NULL, 10);
                if (trace_register >= 32) {
                    fprintf(stderr, "Invalid register\n");
                    exit(2);
                    break;
                }
                break;
            default:
                usage(argv[0]);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "ROM not specified!\n");
        usage(argv[0]);
    }

    rom_file = argv[optind];

    if (mem_bits < 10 || mem_bits > 32) {
        fprintf(stderr, "Invalid address bits. Valid range: [10, 32]\n");
        exit(2);
    }

    int rom_fd = open(rom_file, O_RDONLY);
    if (rom_fd < 0) {
        perror("open");
        exit(2);
    }

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

    if (rom_stat.st_size >= (1<<mem_bits)) {
        fprintf(stderr, "Address size not enough for ROM! Please choose a bigger address size.\n");
        exit(2);
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

    if (trace_functions) {
        fprintf(trace_functions, "%08x: %08x\n", cpu_state.pc, cpu_state.pc);
    }

    cpu_dump(0);

    while (!cpu_halt()) {
        clock();
        cpu_dump(0);

        if (debug) {
            (void) getchar();
        }
    }

    cpu_dump(0);

    if (trace_functions) {
        fflush(trace_functions);
        fclose(trace_functions);
    }

    debug = 1;
    cpu_dump(0);

    fprintf(stderr, "Total cycles: %lu\n", cpu_state.total_cycles);
    fprintf(stderr, "Total instructions: %lu\n", cpu_state.total_instructions);
    fprintf(stderr, "IPC: %0.2f\n", (double)cpu_state.total_instructions / (double)cpu_state.total_cycles);

    return 0;
}
