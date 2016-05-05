#include "execute.h"

#include "cpu.h"
#include "issue.h"

enum {
    ALU_RS_COUNT = ISSUE_WIDTH,
};

struct rs_alu
w_rs_alu[ALU_RS_COUNT],
r_rs_alu[ALU_RS_COUNT];

int execute_alu(struct rs_alu rs) {
    for (int i = 0; i < ALU_RS_COUNT; ++i) {
        if (w_rs_alu[i].busy)
            continue;

        w_rs_alu[i] = rs;
        return 0;
    }

    cpu_stats.sc_execute += 1;
    cpu_stats.sc_execute_alu += 1;
    return 1;
}

static void
copy_front(void)
{

}

static void
copy_back(void)
{

}


void execute_clock(void) {
    copy_front();

    copy_back();
}
