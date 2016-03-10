#include "cpu_state.h"
#include "if_stage.h"
#include "memory.h"

struct if_id_state if_id_state;

void if_stage(void) {
    if_id_state.instruction = make_instruction(memory(0, cpu_state.pc, 0));
    if_id_state.next_pc = make_address(address_val(cpu_state.pc) + 4);
}
