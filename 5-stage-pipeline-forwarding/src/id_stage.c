#include "id_stage.h"

#include "cpu_state.h"
#include "ex_stage.h"
#include "if_stage.h"
#include "wb_stage.h"
#include "mem_stage.h"
#include "register.h"

#include <assert.h>

#define ASSERT_OR_ILLEGAL(COND) do { if (!(COND)) goto invalid_instruction; } while (0)

struct id_state_t id_state;

static int operand_fetch(address_t reg, word_t *op, int *op_sel);
static int operand_immediate(word_t imm, word_t *op, int *op_sel);
static word_t extend_immediate(word_t imm);

static void stall_if(int cycles);
static void stall_id(void);

static unsigned int call_depth = 1;
static void trace_call(void);
static void trace_return(void);

void id_stage(void) {
    word_t const bits = id_state.instruction;
    word_t const opcode = BITS(bits, 26, 31);
    word_t const rd = BITS(bits, 21, 25);
    word_t const ra = BITS(bits, 16, 20);
    word_t const rb = BITS(bits, 11, 15);
    word_t const imm16 = (h_word_t) BITS(bits, 0, 15);
    word_t const imm5 = BITS(bits, 0, 4);

    word_t const function = BITS(bits,  0, 10);
    word_t const br_cond = BITS(bits, 21, 24);

    int const br_link = BITS(bits, 18, 18) != 0;
    int const br_absolute = BITS(bits, 19, 19) != 0;
    int const br_delay = BITS(bits, 20, 20) != 0;
    int const br_cond_delay = BITS(bits, 25, 25) != 0;
    int const use_carry = BITS(bits, 27, 27) != 0;
    int const keep_carry = BITS(bits, 28, 28) != 0;
    int const is_imm_instr = BITS(bits, 29, 29) != 0;

    // The PC passes through
    ex_state.pc = id_state.pc;

    // Disable control signals that can change state
    ex_state.branch_enable = 0;
    ex_state.carry_write_enable = 0;
    ex_state.mem_write_enable = 0;
    ex_state.mem_access = 0;
    ex_state.wb_write_enable = 0;

    switch (opcode) {
    // ADD, ADDC, ADDK, ADDKC
    case 0x00: // ADD
    case 0x01: // RSUB
    case 0x02: // ADDC
    case 0x03: // RSUBC
    case 0x04: // ADDK
    case 0x06: // ADDKC
    case 0x07: // RSUBKC
        ASSERT_OR_ILLEGAL(function == 0);
        // fall through

    case 0x05: // RSUBK/CMP/CMPU
        ASSERT_OR_ILLEGAL(function == 0 || function == 1 || function == 3);
        // fall through

    case 0x08: // ADDI
    case 0x09: // RSUBI
    case 0x0A: // ADDIC
    case 0x0B: // RSUBIC
    case 0x0C: // ADDIK
    case 0x0D: // RSUBIK
    case 0x0E: // ADDIKC
    case 0x0F: // RSUBIKC
    {
        if (operand_fetch(ra, &ex_state.op_a, &ex_state.sel_op_a))
            return;
        if (is_imm_instr)
            operand_immediate(imm16, &ex_state.op_b, &ex_state.sel_op_b);
        else if (operand_fetch(rb, &ex_state.op_b, &ex_state.sel_op_b))
            return;

        ex_state.wb_dest_register = rd;
        ex_state.wb_write_enable = 1;
        ex_state.wb_select_data = WB_SEL_EX;

        if (opcode & 0x01)
            ex_state.alu_control = use_carry ? EX_ALU_SUBC : EX_ALU_SUB;
        else
            ex_state.alu_control = use_carry ? EX_ALU_ADDC : EX_ALU_ADD;

        ex_state.carry_write_enable = !keep_carry;

        // We're dealing with a CMP/CMPU
        if (opcode == 0x05 && (function == 1 || function == 3)) {
            // CMP has swapped operands
            word_t tmp = ex_state.op_a;
            ex_state.op_a = ex_state.op_b;
            ex_state.op_b = tmp;

            ex_state.is_signed = function == 1;
            ex_state.alu_control = EX_ALU_CMP;
        }

        msr.i = 0;
        break;
    }

    case 0x10: // MUL, MULH, MULHU, MULHSU
        ASSERT_OR_ILLEGAL(function < 4);
        // fall through

    case 0x18: // MULI
    {
        if (operand_fetch(ra, &ex_state.op_a, &ex_state.sel_op_a))
            return;
        if (is_imm_instr)
            operand_immediate(imm16, &ex_state.op_b, &ex_state.sel_op_b);
        else if (operand_fetch(rb, &ex_state.op_b, &ex_state.sel_op_b))
            return;

        ex_state.wb_dest_register = rd;
        ex_state.wb_write_enable = 1;
        ex_state.wb_select_data = WB_SEL_EX;

        ex_state.alu_control = EX_ALU_MUL;

        if (!is_imm_instr) {
            switch (function) {
            case 0:
                ex_state.alu_control = EX_ALU_MUL;
                break;
            case 1:
                ex_state.alu_control = EX_ALU_MULH;
                break;
            case 2:
                ex_state.alu_control = EX_ALU_MULHSU;
                break;
            case 3:
                ex_state.alu_control = EX_ALU_MULHU;
                break;
            default:
                ABORT_WITH_MSG("should never happen");
            }
        }

        msr.i = 0;
        break;
    }

    case 0x11: // BSRA, BSLA, BSRL, BSLL
    case 0x19: // BSRAI, BSLAI, BSRLI, BSLLI
    {
        if (operand_fetch(ra, &ex_state.op_a, &ex_state.sel_op_a))
            return;
        if (is_imm_instr)
            // FIXME: potential bug: imm5 will be sign extended.
            // We cut off all but the last 5 bits in the ALU, though.
            // Just a reminder.
            operand_immediate(imm5, &ex_state.op_b, &ex_state.sel_op_b);
        else if (operand_fetch(rb, &ex_state.op_b, &ex_state.sel_op_b))
            return;

        ex_state.wb_write_enable = 1;
        ex_state.wb_dest_register = rd;
        ex_state.wb_select_data = WB_SEL_EX;

        ex_state.is_signed = (function & (1<<9)) != 0;
        ex_state.alu_control = (function & (1<<10)) ? EX_ALU_SHIFT_LEFT : EX_ALU_SHIFT_RIGHT;

        msr.i = 0;
        break;
    }

    case 0x24: // SRA, SRC, SRL, SEXT8, SEXT16
    {
        if (operand_fetch(ra, &ex_state.op_a, &ex_state.sel_op_a))
            return;
        operand_immediate(1, &ex_state.op_b, &ex_state.sel_op_b);

        ex_state.wb_write_enable = 1;
        ex_state.wb_dest_register = rd;
        ex_state.wb_select_data = WB_SEL_EX;

        ex_state.alu_control = EX_ALU_SHIFT_RIGHT;

        switch (function) {
        case 0x001: // SRA
            ex_state.is_signed = 1;
            ex_state.carry_write_enable = 1;
            break;
        case 0x021: // SRC
            ex_state.use_carry = 1;
            ex_state.carry_write_enable = 1;
            break;
        case 0x041: // SRL
            ex_state.carry_write_enable = 1;
            break;
        case 0x060: // SEXT8
            ex_state.alu_control = EX_ALU_SEXT;
            operand_immediate(7, &ex_state.op_b, &ex_state.sel_op_b);
            break;
        case 0x061: // SEXT16
            ex_state.alu_control = EX_ALU_SEXT;
            operand_immediate(15, &ex_state.op_b, &ex_state.sel_op_b);
            break;
        default:
            ASSERT_OR_ILLEGAL(0);
            break;
        }

        msr.i = 0;
        break;
    }

    case 0x20: // OR
    case 0x21: // AND
    case 0x22: // XOR
    case 0x23: // ANDN
        ASSERT_OR_ILLEGAL(function == 0);
        // fall through

    case 0x28: // ORI
    case 0x29: // ANDI
    case 0x2A: // XORI
    case 0x2B: // ANDNI
    {
        if (operand_fetch(ra, &ex_state.op_a, &ex_state.sel_op_a))
            return;
        if (is_imm_instr)
            operand_immediate(imm16, &ex_state.op_b, &ex_state.sel_op_b);
        else if (operand_fetch(rb, &ex_state.op_b, &ex_state.sel_op_b))
            return;

        ex_state.wb_dest_register = rd;
        ex_state.wb_select_data = WB_SEL_EX;
        ex_state.wb_write_enable = 1;

        switch (BITS(opcode, 0, 1)) {
            case 0:
                ex_state.alu_control = EX_ALU_OR;
                break;
            case 1:
                ex_state.alu_control = EX_ALU_AND;
                break;
            case 2:
                ex_state.alu_control = EX_ALU_XOR;
                break;
            case 3:
                ex_state.alu_control = EX_ALU_ANDN;
                break;
            default:
                ABORT_WITH_MSG("should never happen!");
        }

        msr.i = 0;
        break;
    }

    case 0x2C: // IMM
    {
        rIMM = (h_word_t) imm16;
        msr.i = 1;

        break;
    }

    case 0x30: // LBU
    case 0x31: // LHU
    case 0x32: // LW
        ASSERT_OR_ILLEGAL(function == 0);
        // fallthrough

    case 0x38: // LBUI
    case 0x39: // LHUI
    case 0x3A: // LWI
    {
        if (operand_fetch(ra, &ex_state.op_a, &ex_state.sel_op_a))
            return;
        if (is_imm_instr)
            operand_immediate(imm16, &ex_state.op_b, &ex_state.sel_op_b);
        else if (operand_fetch(rb, &ex_state.op_b, &ex_state.sel_op_b))
            return;

        ex_state.wb_dest_register = rd;
        ex_state.wb_select_data = WB_SEL_MEM;
        ex_state.wb_write_enable = 1;

        ex_state.alu_control = EX_ALU_ADD;

        ex_state.mem_access = 1;

        switch (opcode & 0x03) {
            case 0:
                ex_state.mem_mode = MEM_BYTE;
                break;
            case 1:
                ex_state.mem_mode = MEM_HALF;
                break;
            case 2:
                ex_state.mem_mode = MEM_WORD;
                break;
            default:
                ABORT_WITH_MSG("should never happen!");
        }

        msr.i = 0;
        break;
    }

    case 0x34: // SB
    case 0x35: // SH
    case 0x36: // SW
        ASSERT_OR_ILLEGAL(function == 0);
        // fallthrough

    case 0x3C: // SBI
    case 0x3D: // SHI
    case 0x3E: // SWI
    {
        if (operand_fetch(ra, &ex_state.op_a, &ex_state.sel_op_a))
            return;
        if (is_imm_instr)
            operand_immediate(imm16, &ex_state.op_b, &ex_state.sel_op_b);
        else if (operand_fetch(rb, &ex_state.op_b, &ex_state.sel_op_b))
            return;
        if (operand_fetch(rd, &ex_state.mem_data, NULL))
            return;

        ex_state.alu_control = EX_ALU_ADD;

        ex_state.mem_access = 1;
        ex_state.mem_write_enable = 1;

        switch (opcode & 0x07) {
            case 4:
                ex_state.mem_mode = MEM_BYTE;
                break;
            case 5:
                ex_state.mem_mode = MEM_HALF;
                break;
            case 6:
                ex_state.mem_mode = MEM_WORD;
                break;
            default:
                ABORT_WITH_MSG("should never happen!");
        }

        msr.i = 0;
        break;
    }

    case 0x25: // MTS
        // fprintf(stderr, "[warn] mts instruction is not and won't be implemented\n");

        msr.i = 0;
        break;

    case 0x26: // BR,  BRL,  BRA,  BRAL,  BRD,  BRLD,  BRAD,  BRALD
        ASSERT_OR_ILLEGAL(function == 0);
        // fallthrough

    case 0x2E: // BRI, BRLI, BRAI, BRALI, BRID, BRLID, BRAID, BRALID
    {
        ex_state.sel_op_a = EX_SELOP_PC;

        if (br_absolute)
            operand_immediate(0, &ex_state.op_a, &ex_state.sel_op_a);
        if (is_imm_instr)
            operand_immediate(imm16, &ex_state.op_b, &ex_state.sel_op_b);
        else if (operand_fetch(rb, &ex_state.op_b, &ex_state.sel_op_b))
            return;

        ex_state.branch_enable = 1;
        ex_state.branch_cond = EX_COND_ALWAYS;
        ex_state.alu_control = EX_ALU_ADD;

        if (br_link) {
            ex_state.wb_dest_register = rd;
            ex_state.wb_select_data = WB_SEL_PC;
            ex_state.wb_write_enable = 1;

            trace_call();
        }

        stall_if(3);
        cpu_state.has_delayed_branch = br_delay;
        msr.i = 0;

        break;
    }

    case 0x27: // BEQ,  BNE,  BLT,  BLE,  BGT,  BGE,  BEQD,  BNED,  BLTD,  BLED,  BGTD,  BGED
        ASSERT_OR_ILLEGAL(function == 0);
        // fallthrough

    case 0x2F: // BEQI, BNEI, BLTI, BLEI, BGTI, BGEI, BEQID, BNEID, BLTID, BLEID, BGTID, BGEID
    {
        ASSERT_OR_ILLEGAL(br_cond <= 5);

        ex_state.sel_op_a = EX_SELOP_PC;

        if (is_imm_instr) {
            operand_immediate(imm16, &ex_state.op_b, &ex_state.sel_op_b);
        } else if (operand_fetch(rb, &ex_state.op_b, &ex_state.sel_op_b)) {
            return;
        }

        if (operand_fetch(ra, &ex_state.branch_op, &ex_state.branch_sel_op)) {
            return;
        }

        ex_state.branch_enable = 1;
        ex_state.branch_cond = br_cond;

        ex_state.alu_control = EX_ALU_ADD;
        ex_state.is_signed = 1;

        stall_if(3);
        cpu_state.has_delayed_branch = br_cond_delay;
        msr.i = 0;

        break;
    }

    case 0x2D: // RTSD
    {
        ASSERT_OR_ILLEGAL(BITS(bits, 21, 25) == 0x10);

        if (operand_fetch(ra, &ex_state.op_a, &ex_state.sel_op_a))
            return;
        if (operand_immediate(imm16, &ex_state.op_b, &ex_state.sel_op_b))
            return;


        ex_state.branch_enable = 1;
        ex_state.branch_cond = EX_COND_ALWAYS;
        ex_state.alu_control = EX_ALU_ADD;

        trace_return();

        stall_if(3);
        cpu_state.has_delayed_branch = 1;
        msr.i = 0;

        break;
    }

    default:
    invalid_instruction:
        ABORT_WITH_MSG("illegal instruction (opcode %02x pc %08x instr %08x)", opcode, id_state.pc, id_state.instruction);
        break;
    };
}

static word_t extend_immediate(word_t imm) {
    assert((h_word_t)imm == imm);

    if (msr.i) {
        return ((word_t)rIMM << 16) | imm;
    } else {
        return (word_t)sign_extend(imm, 32, 15);
    }
}

static int operand_fetch(address_t reg, word_t *op, int *op_sel) {
    if (register_in_use(reg)) {
        stall_id();
        return 1;
    }

    *op = register_read(reg);
    if (op_sel != NULL) {
        *op_sel = EX_SELOP_IMM;
    }

    return 0;
}

static int operand_immediate(word_t imm, word_t *op, int *op_sel) {
    *op = extend_immediate(imm);
    *op_sel = EX_SELOP_IMM;

    return 0;
}

static void stall_if(int cycles) {
    cpu_state.if_stalls = cycles;
}

static void stall_id(void) {
    cpu_state.id_stall = 1;
}

// Debugging purposes

static void trace_call(void) {
    if (!trace_functions)
        return;

    fprintf(trace_functions, "%08x:", id_state.pc);

    for (unsigned int i = 0; i < call_depth; ++i) {
        fprintf(trace_functions, "    ");
    }

    fprintf(trace_functions, "%08x\n", (word_t)((l_word_t)ex_state.op_a + (l_word_t)ex_state.op_b));
    call_depth++;
}

static void trace_return(void) {
    if (!trace_functions)
        return;

    call_depth--;
}
