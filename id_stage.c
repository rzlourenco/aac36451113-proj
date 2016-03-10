#include "if_stage.h"
#include "id_stage.h"

#include <stdio.h>
#include <stdlib.h>

struct id_ex_state id_ex_state;

void id_stage(void) {
    uint32_t bits = instruction_val(if_id_state.instruction);
    int opcode = BITS(bits, 26, 32);
    int rd = BITS(bits, 21, 26);
    int ra = BITS(bits, 16, 21);
    int rb = BITS(bits, 11, 16);
    int imm16 = BITS(bits, 0, 16);
    int imm5 = BITS(bits, 0, 5);

    switch (opcode) {
        /* Arithmetic, logic, and shift */
        case 0x00: // ADD
        case 0x01: // RSUB
        case 0x02: // ADDC
        case 0x03: // RSUBC
        case 0x04: // ADDK
        case 0x05: // RSUBK, CMP, CMPU
        case 0x06: // ADDCK
        case 0x07: // RSUBKC

        case 0x08: // ADDI
        case 0x09: // RSUBI
        case 0x0A: // ADDIC
        case 0x0B: // RSUBIC
        case 0x0C: // ADDIK
        case 0x0D: // RSUBIK
        case 0x0E: // ADDIKC
        case 0x0F: // RSUBIKC

        case 0x10: // MUL, MULH, MULHU, MULHSU

        case 0x11: // BSRA,  BSLA,  BSRL,  BSLL
        case 0x19: // BSRAI, BSLAI, BSRLI, BSLLI

        case 0x24: // SRA, SRC, SRL, SEXT8, SEXT16

        case 0x20: // OR
        case 0x21: // AND
        case 0x22: // XOR
        case 0x23: // ANDN

        case 0x28: // ORI
        case 0x29: // ANDI
        case 0x2A: // XORI
        case 0x2B: // ANDNI

        case 0x2C: // IMM

        case 0x30: // LBU
        case 0x31: // LHU
        case 0x32: // LW

        case 0x34: // SB
        case 0x35: // SH
        case 0x36: // SW

        case 0x38: // LBUI
        case 0x39: // LHUI
        case 0x3A: // LWI

        case 0x3C: // SBI
        case 0x3D: // SHI
        case 0x3E: // SWI

        case 0x26: // BR, BRL, BRA, BRAL, BRD, BRLD, BRAD, BRALD
        case 0x27: // BEQ, BNE, BLT, BLE, BGT, BGE, BEQD, BNED, BLTD, BLED, BGTD, BGED

        case 0x2E: // BRI, BRLI, BRAI, BRALI, BRID, BRLID, BRAID, BRALID
        case 0x2F: // BEQI, BNEI, BLTI, BLEI, BGTI, BGEI, BEQID, BNEID, BGTID, BGEID

        case 0x2D: // RTSD

            break;

        default:
            fprintf(stderr, "Invalid instruction (opcode %d)\n", opcode);
            abort();
    };
}
