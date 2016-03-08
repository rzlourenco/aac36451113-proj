#include "if_stage.h"
#include "id_stage.h"
#include "ex_stage.h"
#include "mem_stage.h"
#include "wb_stage.h"


void clock(void) {
    instruction_fetch();
}

int main(int argc, char **argv) {
    address_t a;
    instruction_t b;

    return 0;
}
