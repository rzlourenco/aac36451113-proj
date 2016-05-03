#include "fetch.h"

#include "common.h"

#include "cpu.h"
#include "memory.h"

#include "branch_predictor.h"
#include "issue.h"


void fetch_clock(void) {
    address_t pc, target;
    word_t instr;
    int taken = 0, delayed = 0, issue = 0;

    while (issue < ISSUE_WIDTH) {
        pc = cpu_state.pc;
        instr = memory_read(pc);
        bp_branch_predict(pc, &taken, &delayed, &target);

        if (issue_queue_instruction(pc, instr, taken, delayed, target))
            return;

        if (taken)
            pc = target;
        else
            pc += sizeof(pc);

        cpu_update_pc(pc);
        issue++;
    }
}
