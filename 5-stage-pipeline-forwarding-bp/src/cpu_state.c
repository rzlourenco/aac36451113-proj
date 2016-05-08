#include "cpu_state.h"

#include "if_stage.h"
#include "id_stage.h"
#include "ex_stage.h"
#include "mem_stage.h"
#include "wb_stage.h"

#include "branch_predictor.h"
#include "register.h"

struct cpu_state_t cpu_state;
struct msr_t msr;
uint16_t rIMM;

int little_endian;

int debug;

int has_breakpoint;
address_t breakpoint;

address_t trace_register;

int has_mem_trace;
address_t mem_trace;

FILE *trace_functions;

void init_cpu(void) {
    cpu_state = (struct cpu_state_t){ 0 };

    cpu_state.pc = 0;

    cpu_state.halt = 0;
    cpu_state.has_delayed_branch = 0;

    cpu_state.if_enable = 1;
    cpu_state.id_enable = 0;
    cpu_state.ex_enable = 0;
    cpu_state.mem_enable = 0;
    cpu_state.wb_enable = 0;

    cpu_state.total_cycles = 0;
    cpu_state.total_instructions = 0;
    cpu_state.branches = 0;
    cpu_state.branches_predicted = 0;

    msr.c = 0;
    msr.i = 0;

    rIMM = 0;

    id_state = (struct id_state_t){ 0 };
    ex_state = (struct ex_state_t){ 0 };
    mem_state = (struct mem_state_t){ 0 };
    wb_state = (struct wb_state_t){ 0 };

    bp_init();
}

int cpu_halt(void) {
    return (!cpu_state.if_enable && !cpu_state.id_enable && !cpu_state.ex_enable &&
           !cpu_state.mem_enable && !cpu_state.wb_enable);
}

void clock(void) {
    if (cpu_state.if_stalls > 0) {
        cpu_state.if_stalls -= 1;
    }
    if (cpu_state.id_stall) {
        cpu_state.id_stall = 0;
    }

    struct id_state_t old_id_state = id_state, new_id_state = {0};
    struct ex_state_t old_ex_state = ex_state, new_ex_state = {0};
    struct mem_state_t old_mem_state = mem_state, new_mem_state = {0};
    struct wb_state_t old_wb_state = wb_state, new_wb_state = {0};


    if (cpu_state.wb_enable) {
        cpu_state.wb_enable = 0;

        wb_stage();
    }

    if (cpu_state.mem_enable) {
        cpu_state.wb_enable = 1;
        cpu_state.mem_enable = 0;

        //wb_state = (struct wb_state_t){0};
        mem_stage();

        new_wb_state = wb_state;
        wb_state = old_wb_state;
    }

    if (cpu_state.ex_enable) {
        cpu_state.mem_enable = 1;
        cpu_state.ex_enable = 0;

        //mem_state = (struct mem_state_t){0};
        ex_stage();

        new_mem_state = mem_state;
        mem_state = old_mem_state;
    }

    if (cpu_state.id_enable) {
        cpu_state.ex_enable = 1;
        cpu_state.id_enable = 0;

        //ex_state = (struct ex_state_t){0};
        id_stage();

        if (cpu_state.id_stall)
            cpu_state.ex_enable = 0;

        new_ex_state = ex_state;
        ex_state = old_ex_state;
    }

    if (cpu_state.if_enable) {
        cpu_state.id_enable = 1;

        //id_state = (struct id_state_t){0};
        if_stage();

        new_id_state = id_state;
        id_state = old_id_state;
    }

    id_state = new_id_state;
    ex_state = new_ex_state;
    mem_state = new_mem_state;
    wb_state = new_wb_state;

    // Advance register-in-use state
    register_clock();

    ++cpu_state.total_cycles;
}

void cpu_dump(int signal) {
    if (!debug && signal == 0)
        return;

    fprintf(stderr, "\n");

    fprintf(stderr, "\tAfter %zu clock cycle(s)\n", cpu_state.total_cycles);
    fprintf(stderr,
            "\tpc=%08x msr{ .c=%d .i=%d } rIMM=%04x if_stalls=%d has_delayed_branch=%d\n"
            "",
            cpu_state.pc,
            msr.c,
            msr.i,
            rIMM,
            cpu_state.if_stalls,
            cpu_state.has_delayed_branch);

    fprintf(stderr, "\tif(%d) pc<-%d(%08x %08x)\n",
            cpu_state.if_enable,
            if_state.sel_pc,
            cpu_state.pc + 4,
            if_state.branch_pc
    );

    fprintf(stderr, "\tid(%d) id_stall=%d pc=%08x instruction=%08x bp(%d %d %08x)\n",
            cpu_state.id_enable,
            cpu_state.id_stall,
            id_state.pc,
            id_state.instruction,
            id_state.br_pred_taken,
            id_state.br_pred_delayed,
            id_state.br_pred_target
    );

    fprintf(stderr, "\tex(%d) pc=%08x branch_enable=%d branch_cond=%d\n"
                    "\t      op_a<-%d(%08x %08x %08x %08x)\n"
                    "\t      op_b<-%d(%08x %08x %08x %08x)\n"
                    "\t      branch_op<-%d(%08x %08x %08x %08x)\n"
                    "\t      delayed:%d bp(%d %d %08x)\n"
            ,
            cpu_state.ex_enable,
            ex_state.pc,
            ex_state.branch_enable,
            ex_state.branch_cond,
            ex_state.sel_op_a,
            ex_state.op_a,
            ex_state.pc,
            mem_state.alu_result,
            wb_get_result(),
            ex_state.sel_op_b,
            ex_state.op_b,
            ex_state.pc,
            mem_state.alu_result,
            wb_get_result(),
            ex_state.branch_sel_op,
            ex_state.branch_op,
            ex_state.pc,
            mem_state.alu_result,
            wb_get_result(),
            ex_state.branch_delayed,
            ex_state.br_pred_taken,
            ex_state.br_pred_delayed,
            ex_state.br_pred_target
    );

    fprintf(stderr, "\tmem(%d) pc=%08x write_enable=%d address=%08x data=%08x\n",
            cpu_state.mem_enable,
            mem_state.pc,
            mem_state.write_enable,
            mem_state.alu_result,
            mem_state.data
    );

    fprintf(stderr, "\twb(%d) pc=%08x write_enable=%d r%d<-%d(%08x %08x %08x)\n",
            cpu_state.wb_enable,
            wb_state.pc,
            wb_state.write_enable,
            wb_state.dest_register,
            wb_state.select_data,
            wb_state.pc,
            wb_state.alu_result,
            wb_state.memory_out
    );

    fprintf(stderr, "\n");
    register_dump();
    fprintf(stderr, "\n");

    if (signal) {
        exit(signal);
    }

    fflush(stderr);
}
