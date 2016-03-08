#include "if_stage.h"
#include "id_stage.h"

#include <stdio.h>
#include <stdlib.h>

struct id_ex_state id_ex_state;

void instruction_decode(void) {
    uint32_t bits = instruction_val(if_id_state.instruction);
    int opcode = BITS(bits, 26, 32);
    int rd = BITS(bits, 21, 26);
    int ra = BITS(bits, 16, 21);
    int rb = BITS(bits, 11, 16);
    int imm16 = BITS(bits, 0, 16);
    int imm5 = BITS(bits, 0, 5);

    switch (opcode) {
        /* Arithmetic, logic, and shift */
        case 0b000000: // ADD
        case 0b000010: // ADDC
        case 0b000100: // ADDK
        case 0b000110: // ADDCK
        case 0b001000: // ADDI
        case 0b001010: // ADDIC
        case 0b001100: // ADDIK
        case 0b001110: // ADDIKC
        case 0b000001: // RSUB
        case 0b000011: // RSUBC
        case 0b000101: // RSUBK, CMP, CMPU
        case 0b000111: // RSUBKC
        case 0b001001: // RSUBI
        case 0b001011: // RSUBIC
        case 0b001101: // RSUBIK
        case 0b001111: // RSUBIKC
        case 0b010000: // MUL, MULH, MULHU, MULHSU

        case 0b010001: // BSRA,  BSLA,  BSRL,  BSLL
        case 0b011001: // BSRAI, BSLAI, BSRLI, BSLLI
        case 0b100100: // SRA, SRC, SRL, SEXT8, SEXT16

        case 0b100000: // OR
        case 0b101000: // ORI
        case 0b100001: // AND
        case 0b101001: // ANDI
        case 0b100010: // XOR
        case 0b101010: // XORI
        case 0b100011: // ANDN
        case 0b101011: // ANDNI
        case 0b101100: // IMM

        case 0b110000: // LBU
        case 0b111000: // LBUI
        case 0b110001: // LHU
        case 0b111001: // LHUI
        case 0b110010: // LW
        case 0b111010: // LWI
        case 0b110100: // SB
        case 0b111100: // SBI
        case 0b110101: // SH
        case 0b111101: // SHI
        case 0b110110: // SW
        case 0b111110: // SWI

        case 0b100110: // BR, BRL, BRA, BRAL, BRD, BRLD, BRAD, BRALD
        case 0b101110: // BRI, BRLI, BRAI, BRALI, BRID, BRLID, BRAID, BRALID
        case 0b100111: // BEQ, BNE, BLT, BLE, BGT, BGE, BEQD, BNED, BLTD, BLED, BGTD, BGED
        case 0b101111: // BEQI, BNEI, BLTI, BLEI, BGTI, BGEI, BEQID, BNEID, BGTID, BGEID
        case 0b101101: // RTSD
            break;

        default:
            fprintf(stderr, "Invalid instruction (opcode %d)\n", opcode);
            abort();
    };
}
