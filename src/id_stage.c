#include "cpu_state.h"
#include "if_stage.h"
#include "id_stage.h"
#include "ex_stage.h"
#include "reg_file.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <wb_stage.h>

#define ASSERT_OR_ILLEGAL(COND) do { if (!(COND)) goto invalid_instruction; } while (0)

struct id_state_t id_state;

static word_t extend_immediate(word_t imm);

void id_stage(void) {
    word_t const bits = id_state.instruction;
    word_t const opcode = BITS(bits, 26, 31);
    word_t const rd = BITS(bits, 21, 25);
    word_t const ra = BITS(bits, 16, 20);
    word_t const rb = BITS(bits, 11, 15);
    word_t const imm16 = (h_word_t) BITS(bits, 0, 15);
    word_t const imm5 = BITS(bits, 0, 4);

    word_t const function = BITS(bits,  0, 10);
    word_t const br_link = BITS(bits, 18, 18) != 0;
    word_t const br_absolute = BITS(bits, 19, 19) != 0;
    word_t const br_delay = BITS(bits, 20, 20) != 0;
    word_t const br_cond = BITS(bits, 21, 25);
    word_t const use_carry = BITS(bits, 27, 27) != 0;
    word_t const keep_carry = BITS(bits, 28, 28) != 0;
    word_t const has_imm = BITS(bits, 29, 29) != 0;

    // Clear EX stage (issue a no-op)
    ex_state = (struct ex_state_t){ 0 };

    // The PC passes through
    ex_state.pc = id_state.pc;

    switch (opcode) {
        // ADD, ADDC, ADDK, ADDKC
        case 0x00: // ADD
        case 0x01: // RSUB
        case 0x02: // ADDC
        case 0x03: // RSUBC
        case 0x04: // ADDK
        case 0x06: // ADDKC
        case 0x07: // RSUBKC
    cpu_state.ex_enable = cpu_state.id_enable;
            ASSERT_OR_ILLEGAL(function == 0);
            // fall through

        case 0x05: // RSUBK/CMP/CMPU
        case 0x08: // ADDI
        case 0x09: // RSUBI
        case 0x0A: // ADDIC
        case 0x0B: // RSUBIC
        case 0x0C: // ADDIK
        case 0x0D: // RSUBIK
        case 0x0E: // ADDIKC
        case 0x0F: // RSUBIKC
        {
            ex_state.wb_dest_register = rd;
            ex_state.wb_write_enable = 1;
            ex_state.wb_select_data = WB_SEL_EX;

            ex_state.alu_control = EX_ALU_ADD;
            ex_state.op_a = reg_file_read(ra);
            ex_state.op_b = has_imm ? extend_immediate(imm16) : reg_file_read(rb);
            ex_state.op_c = 0;

            // Subtract or compare
            if (opcode & 0x01) {
                ex_state.op_b = ~ex_state.op_b;
                ex_state.op_c = 1;
            }

            if (use_carry) {
                ex_state.op_c = msr.c;
            }
            ex_state.carry_write_enable = !keep_carry;

            msr.i = 0;

            // We're dealing with a CMP/CMPU
            if (opcode == 0x05) {
                ASSERT_OR_ILLEGAL(function == 1 || function == 3);

                ex_state.cmp_unsigned = function == 3;
                ex_state.alu_control = EX_ALU_CMP;
            }

            break;
        }

        case 0x10: // MUL, MULH, MULHU, MULHSU
        case 0x18: // MULI
        {
            ABORT_MSG("not implemented");
            break;
        }

        case 0x11: // BSRA, BSLA, BSRL, BSLL
        case 0x19: // BSRAI, BSLAI, BSRLI, BSLLI
        {
            ABORT_MSG("not implemented");
            break;
        }

        case 0x24: // SRA, SRC, SRL, SEXT8, SEXT16
        {
            ABORT_MSG("not implemented");
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
            ex_state.wb_dest_register = rd;
            ex_state.wb_select_data = WB_SEL_EX;
            ex_state.wb_write_enable = 1;

            ex_state.op_a = reg_file_read(ra);
            ex_state.op_b = has_imm ? extend_immediate(imm16) : reg_file_read(rb);

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
                    ex_state.alu_control = EX_ALU_AND;
                    break;
                default:
                    ABORT_MSG("should never happen!");
            }

            // ANDN negates the second operand
            if (opcode & 0x03) {
                ex_state.op_b = ~ex_state.op_b;
            }

            msr.i = 0;
            break;
        }

        case 0x2C: // IMM
        {
            msr.i = 1;
            rIMM = (h_word_t)imm16;

            break;
        }

        case 0x30: // LBU
        case 0x38: // LBUI
        {
            ABORT_MSG("not implemented");
            break;
        }

        case 0x31: // LHU
        case 0x39: // LHUI
            ABORT_MSG("not implemented");
            break;

        case 0x32: // LW
        case 0x3A: // LWI
            ABORT_MSG("not implemented");
            break;

        case 0x34: // SB
        case 0x3C: // SBI
            ABORT_MSG("not implemented");
            break;

        case 0x35: // SH
        case 0x3D: // SHI
            ABORT_MSG("not implemented");
            break;

            ex_state.op_c = 0;
        case 0x36: // SW
        case 0x3E: // SWI
            ABORT_MSG("not implemented");
            break;

        case 0x26: // BR,  BRL,  BRA,  BRAL,  BRD,  BRLD,  BRAD,  BRALD
            ASSERT_OR_ILLEGAL(function == 0);
            // fallthrough

        case 0x2E: // BRI, BRLI, BRAI, BRALI, BRID, BRLID, BRAID, BRALID
        {
            cpu_state.if_stalls = 2;

            ex_state.branch_enable = 1;
            ex_state.branch_cond = EX_COND_ALWAYS;
            ex_state.alu_control = EX_ALU_ADD;

            if (br_link) {
                ex_state.wb_dest_register = rd;
                ex_state.wb_select_data = WB_SEL_PC;
                ex_state.wb_write_enable = 1;
            }

            if (br_delay) {
                cpu_state.delayed = 1;
            }

            ex_state.op_a = br_absolute ? 0 : id_state.pc;
            ex_state.op_b = has_imm ? extend_immediate(imm16) : reg_file_read(rb);

            msr.i = 0;
            break;
        }

        case 0x27: // BEQ,  BNE,  BLT,  BLE,  BGT,  BGE,  BEQD,  BNED,  BLTD,  BLED,  BGTD,  BGED
            ASSERT_OR_ILLEGAL(function == 0);
            // fallthrough
        case 0x2F: // BEQI, BNEI, BLTI, BLEI, BGTI, BGEI, BEQID, BNEID, BLTID, BLEID, BGTID, BGEID
        {
            ABORT_MSG("not implemented");
            break;
        }

        case 0x2D: // RTSD
            ABORT_MSG("not implemented");
            break;

        default:
        invalid_instruction:
            ABORT_MSG("illegal instruction");
            break;
    };

    cpu_state.ex_enable = 1;
    cpu_state.id_enable = 0;
}

static word_t extend_immediate(word_t imm) {
    assert((h_word_t)imm == imm);

    if (msr.i) {
        return (rIMM << 16) | imm;
    } else {
        return SIGN_EXTEND(imm, 16);
    }
}
