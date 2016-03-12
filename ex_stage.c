#include "ex_stage.h"
#include "id_stage.h"

#include <stdlib.h>

struct ex_mem_state ex_mem_state;

void ex_stage(void) {
    word_t a;

    switch (id_ex_state.op_sel) {
        default:
            break;
    }

    ex_mem_state.a = a;
}
