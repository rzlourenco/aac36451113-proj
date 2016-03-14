#include "cpu_state.h"
#include "if_stage.h"
#include "id_stage.h"
#include "reg_file.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT_OR_ILLEGAL(COND) do { if (!(COND)) goto illegal_instruction; } while (0)

struct id_ex_state_t id_ex_state;

void id_stage(void) {
    uint32_t const bits = (uint32_t) if_id_state.instruction;
    int opcode = BITS(bits, 26, 31);
    int rd = BITS(bits, 21, 25);
    int ra = BITS(bits, 16, 20);
    int rb = BITS(bits, 11, 15);
    word_t imm16 = (int16_t) BITS(bits, 0, 15);
    word_t imm5 = BITS(bits, 0, 4);

    int c = (opcode & 0x02) != 0;
    int k = (opcode & 0x04) != 0;
    int i = (opcode & 0x08) != 0;

    printf("%08x [%08x]:\t", if_id_state.pc, bits);

    switch (opcode) {
        case 0x00:
        case 0x02:
        case 0x04:
        case 0x06:
        case 0x08:
        case 0x0A:
        case 0x0C:
        case 0x0E: {
            // ADD, ADDC, ADDK, ADDKC, ADDI, ADDIC, ADDIK, ADDIKC
            if (i == 0) {
                ASSERT_OR_ILLEGAL(BITS(bits, 0, 10) == 0);
            }

            printf("add");
            if (i != 0) printf("i");
            if (k != 0) printf("k");
            if (c != 0) printf("c");
            printf("\tr%d, r%d, ", rd, ra);

            if (i != 0) printf("%d", imm16);
            else printf("r%d", rb);

            break;
        }

        case 0x01:
        case 0x03:
        case 0x07:
        case 0x09:
        case 0x0B:
        case 0x0D:
        case 0x0F: {
            // RSUB, RSUBC, RSUBKC, RSUBI, RSUBIC, RSUBIK, RSUBIKC
            ASSERT_OR_ILLEGAL(BITS(bits, 0, 10) == 0);

            printf("rsub");
            if (i != 0) printf("i");
            if (k != 0) printf("k");
            if (c != 0) printf("c");

            printf("\tr%d, r%d, ", rd, ra);

            if (i != 0) printf("%d", imm16);
            else printf("r%d", rb);

            break;
        }

        case 0x05:
            ASSERT_OR_ILLEGAL(BITS(bits, 0, 10) == 0x001 || BITS(bits, 0, 10) == 0x003);

            printf("cmp");

            if (BITS(bits, 1, 1))
                printf("u");

            printf("\tr%d, r%d, r%d", rd, ra, rb);
            break;

        case 0x10: // MUL, MULH, MULHU, MULHSU
            printf("MUL|MULH|MULHU|MULHSU");
            break;

        case 0x11: // BSRA, BSLA, BSRL, BSLL
            printf("BSRA|BSLA|BSRL|BSLL");
            break;
        case 0x19: // BSRAI, BSLAI, BSRLI, BSLLI
            printf("BSRAI|BSLAI|BSRLI|BSLLI");
            break;

        case 0x24: // SRA, SRC, SRL, SEXT8, SEXT16
            printf("SRA|SRC|SRL|SEXT8|SEXT16");
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
            ASSERT_OR_ILLEGAL(BITS(bits, 0, 10) == 0);
            char const *operation[] = { "or", "and", "xor", "andn" };

            printf(operation[BITS(bits, 26, 27)]);

            if (i != 0) {
                printf("i\tr%d, r%d, %d", rd, ra, imm16);
            } else {
                printf("\tr%d, r%d, r%d", rd, ra, rb);
            }

            break;
        }

        case 0x2C: // IMM
            printf("imm\t%d", imm16);
            break;

        case 0x30: // LBU
        case 0x38: // LBUI
            printf("lbu");

            if (i != 0) printf("i");

            break;

        case 0x31: // LHU
        case 0x39: // LHUI
            printf("lhu");

            if (i != 0) printf("i");

            break;

        case 0x32: // LW
        case 0x3A: // LWI
            printf("lw");

            if (i != 0) printf("i");

            break;

        case 0x34: // SB
        case 0x3C: // SBI
            printf("sb");

            if (i != 0) printf("i");

            break;

        case 0x35: // SH
        case 0x3D: // SHI
            printf("sh");

            if (i != 0) printf("i");

            break;

        case 0x36: // SW
        case 0x3E: // SWI
            printf("sw");

            if (i != 0) printf("i");

            break;

        case 0x26: // BR, BRL, BRA, BRAL, BRD, BRLD, BRAD, BRALD
        case 0x2E: // BRI, BRLI, BRAI, BRALI, BRID, BRLID, BRAID, BRALID
            printf("br*");
            break;

        case 0x27: // BEQ, BNE, BLT, BLE, BGT, BGE, BEQD, BNED, BLTD, BLED, BGTD, BGED
        case 0x2F: // BEQI, BNEI, BLTI, BLEI, BGTI, BGEI, BEQID, BNEID, BGTID, BGEID
            printf("b*");
            break;

        case 0x2D: // RTSD
            printf("rtsd");
            break;

        default:
            goto illegal_instruction;
    };

    printf("\n");

    if (cpu_state.halt && !cpu_state.if_enable) {
        cpu_state.id_enable = 0;
    }

    return;

    illegal_instruction:
    fprintf(stderr, "Illegal instruction (opcode %d, instr 0x%08x)\n", opcode, bits);
    abort();
}
