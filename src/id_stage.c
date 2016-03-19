#include "cpu_state.h"
#include "if_stage.h"
#include "id_stage.h"
#include "ex_stage.h"
#include "reg_file.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT_OR_ILLEGAL(COND) do { if (!(COND)) goto invalid_instruction; } while (0)

struct id_ex_state_t id_ex_state;

static word_t sign_extend_imm(word_t imm);

void id_stage(void) {
    uint32_t const bits = (uint32_t) if_id_state.instruction;
    int const opcode = BITS(bits, 26, 31);
    int const rd = BITS(bits, 21, 25);
    int const ra = BITS(bits, 16, 20);
    int const rb = BITS(bits, 11, 15);
    word_t const imm16 = (int16_t) BITS(bits, 0, 15);
    word_t const imm5 = BITS(bits, 0, 4);

    int const c = (opcode & 0x02) != 0;
    int const k = (opcode & 0x04) != 0;
    int const i = (opcode & 0x08) != 0;

    struct id_ex_state_t new_id_ex_state;

    switch (opcode) {
        // ADD, ADDC, ADDK, ADDKC
        case 0x00: // ADD
        case 0x01: // RSUB
        case 0x02: // ADDC
        case 0x03: // RSUBC
        case 0x04: // ADDK
        case 0x06: // ADDKC
        case 0x07: // RSUBKC
            ASSERT_OR_ILLEGAL(BITS(bits, 0, 10) == 0);
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
            new_id_ex_state.dest_address = rd;
            new_id_ex_state.select_operation = opcode & 0x01 ? ALU_SUB : ALU_ADD;
            new_id_ex_state.op_a = reg_file_read(ra);
            new_id_ex_state.op_b = i ? sign_extend_imm(imm16) : reg_file_read(rb);
            new_id_ex_state.op_c = c ? msr.c : 0;
            new_id_ex_state.keep_carry = k;
            new_id_ex_state.write_enabled = 1;
            new_id_ex_state.is_mem = 0;

            // We're dealing with a CMP/CMPU
            if (opcode == 0x05 && (BITS(bits, 0, 10) == 0x001 || BITS(bits, 0, 10) == 0x003)) {
                new_id_ex_state.select_operation = ALU_CMP;
                new_id_ex_state.write_enabled = 0;
            }

            break;
        }

        case 0x10:
            // MUL, MULH, MULHU, MULHSU
            break;

        case 0x11:
        case 0x19:
            // BSRA, BSLA, BSRL, BSLL, BSRAI, BSLAI, BSRLI, BSLLI
            break;

        case 0x24:
            // SRA, SRC, SRL, SEXT8, SEXT16
            break;

        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x28:
        case 0x29:
        case 0x2A:
        case 0x2B: {
            // OR, AND, XOR, ANDN, ORI, ANDI, XORI, ANDNI
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
            break;

        case 0x31: // LHU
        case 0x39: // LHUI
            break;

        case 0x32: // LW
        case 0x3A: // LWI
            break;

        case 0x34: // SB
        case 0x3C: // SBI
            break;

        case 0x35: // SH
        case 0x3D: // SHI
            break;

        case 0x36: // SW
        case 0x3E: // SWI
            break;

        case 0x26: // BR, BRL, BRA, BRAL, BRD, BRLD, BRAD, BRALD
        case 0x2E: // BRI, BRLI, BRAI, BRALI, BRID, BRLID, BRAID, BRALID
            break;

        case 0x27: // BEQ, BNE, BLT, BLE, BGT, BGE, BEQD, BNED, BLTD, BLED, BGTD, BGED
        case 0x2F: // BEQI, BNEI, BLTI, BLEI, BGTI, BGEI, BEQID, BNEID, BGTID, BGEID
            break;

        case 0x2D: // RTSD
            break;

        default:
        invalid_instruction:
            fprintf(stderr, "Illegal instruction (opcode %d pc %08x instr %08x)\n", opcode, if_id_state.pc, bits);
            abort();
    };

    id_ex_state = new_id_ex_state;

    cpu_state.id_enable = cpu_state.if_enable;
    cpu_state.ex_enable = 1;
}


static word_t sign_extend_imm(word_t imm) {
    assert((uword_t) (half_word_t) imm == (uword_t) imm);

    if (msr.i) {
        return rIMM << 16 | imm;
    } else {
        return (word_t) (half_word_t) imm;
    }
}
