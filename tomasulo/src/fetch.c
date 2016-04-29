#include "fetch.h"

#include "common.h"

#include "cpu.h"
#include "memory.h"

#include "branch_predictor.h"
#include "issue.h"


void fetch_clock(void) {
    address_t pc, bp_target;
    word_t instr;
    int predicted = 0, issue = 0;

    while (issue < ISSUE_WIDTH) {
        pc = cpu_state.pc;
        instr = memory_read(pc);
        predicted = !bp_branch_predict(pc, &bp_target);

        if (issue_queue_instruction(pc, instr, predicted, bp_target))
            return;

        if (predicted)
            pc = bp_target;
        else
            pc += sizeof(pc);

        cpu_update_pc(pc);
        issue++;
    }
}
