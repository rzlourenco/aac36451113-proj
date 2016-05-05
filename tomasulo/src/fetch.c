#include "fetch.h"

#include "common.h"

#include "cpu.h"
#include "memory.h"

#include "branch_predictor.h"
#include "issue.h"

static int is_delay_slot = 0;
static address_t delayed_branch_target = 0;

void fetch_clock(void) {
    int issued = 0;

    address_t pc;

    while (issued < ISSUE_WIDTH) {
        word_t instr;
        int taken = 0, delayed = 0;
        address_t target = 0;

        pc = cpu_get_pc();
        instr = memory_read(pc);

        if (!is_delay_slot) {
            bp_branch_predict(pc, &taken, &delayed, &target);
        }

        if (issue_queue_instruction(pc, instr, taken, delayed, target))
            break;

        if (is_delay_slot) {
            is_delay_slot = 0;
            taken = 1;
            target = delayed_branch_target;
        }

        is_delay_slot = 0;

        if (taken) {
            if (delayed) {
                pc += sizeof(pc);

                is_delay_slot = 1;
                delayed_branch_target = target;
            } else {
                pc = target;
            }
        } else
            pc += sizeof(pc);

        cpu_update_pc(pc);
        issued++;
    }
}
