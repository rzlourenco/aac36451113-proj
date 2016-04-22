#include "cpu_state.h"

#include "register.h"

void init_cpu(void) {

}

int cpu_halt(void) {
    return 1;
}

void clock(void) {
    

    // Advance register-in-use state
    register_clock();
}

void cpu_dump(int signal) {

}
