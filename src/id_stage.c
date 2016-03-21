#include "cpu_state.h"
#include "if_stage.h"
#include "id_stage.h"
#include "ex_stage.h"
#include "reg_file.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT_OR_ILLEGAL(COND) do { if (!(COND)) goto invalid_instruction; } while (0)

struct id_state_t id_state;

static word_t extend_immediate(word_t imm);

void id_stage(void) {
    int32_t const bits = (uint32_t) id_state.instruction;
    int const opcode = BITS(bits, 26, 31);
    int const rd = BITS(bits, 21, 25);
    int const ra = BITS(bits, 16, 20);
    int const rb = BITS(bits, 11, 15);
    word_t const imm16 = (int16_t) BITS(bits, 0, 15);
    word_t const imm5 = BITS(bits, 0, 4);

    int const function = BITS(bits,  0, 10);
    int const br_link = BITS(bits, 18, 18) != 0;
    int const br_abs = BITS(bits, 19, 19) != 0;
    int const br_delayed = BITS(bits, 20, 20) != 0;
    int const cmp = BITS(bits, 21, 25);
    int const use_carry = BITS(bits, 27, 27) != 0;
    int const keep_carry = BITS(bits, 28, 28) != 0;
    int const has_imm = BITS(bits, 29, 29) != 0;

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
            ex_state.alu_control = EX_ALU_ADD;
            ex_state.op_a = reg_file_read(ra);
            ex_state.op_b = has_imm ? extend_immediate(imm16) : reg_file_read(rb);
            ex_state.op_c = 0;

            // Subtract or compare
            if (opcode & 0x01) {
                ex_state.op_b = ~ex_state.op_b;
                ex_state.op_c = 1;
            }

            ex_state.op_c = use_carry ? msr.c : ex_state.op_c;
            ex_state.carry_write_enable = !keep_carry;
            ex_state.wb_write_enable = 1;
            ex_state.mem_enable = 0;

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
            ABORT_MSG("not implemented");
            break;
        }

        case 0x2C: // IMM
        {
            msr.i = 1;
            rIMM = (uint16_t)imm16;

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

        case 0x36: // SW
        case 0x3E: // SWI
            ABORT_MSG("not implemented");
            break;

        case 0x26: // BR, BRL, BRA, BRAL, BRD, BRLD, BRAD, BRALD
            ASSERT_OR_ILLEGAL(function == 0);
            // fallthrough

        case 0x2E: // BRI, BRLI, BRAI, BRALI, BRID, BRLID, BRAID, BRALID
        {
            if (br_delayed != 0)
                fprintf(stderr, "[warn] delayed branches are not delayed!\n");

            ABORT_MSG("not implemented");

            break;
        }

        case 0x27: // BEQ, BNE, BLT, BLE, BGT, BGE, BEQD, BNED, BLTD, BLED, BGTD, BGED
        case 0x2F: // BEQI, BNEI, BLTI, BLEI, BGTI, BGEI, BEQID, BNEID, BGTID, BGEID
        {
            ASSERT_OR_ILLEGAL(cmp <= 5u);
            ABORT_MSG("not implemented");
            break;
        }

        case 0x2D: // RTSD
            ABORT_MSG("not implemented");
            break;

        default:
        invalid_instruction:
            ABORT_MSG("Illegal instruction (opcode %d pc %08x instr %08x)\n", opcode, id_state.pc, bits);
    };

    cpu_state.id_enable = cpu_state.if_enable;
    cpu_state.ex_enable = 1;
}

static word_t extend_immediate(word_t imm) {
    assert((uword_t) (half_word_t) imm == (uword_t) imm);

    if (msr.i) {
        return rIMM << 16 | imm;
    } else {
        return (word_t) (half_word_t) imm;
    }
}
