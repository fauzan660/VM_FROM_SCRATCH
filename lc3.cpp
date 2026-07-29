// INCLUDES
#include <cstdint>
#include <iostream>
using namespace std;
#define MAX_MEM (1 << 16) /* 65536 */

enum {
  R_R0 = 0,
  R_R1,
  R_R2,
  R_R3,
  R_R4,
  R_R5,
  R_R6,
  R_R7,
  R_PC, /* program counter */
  R_COND,
  R_COUNT

};
enum {
  OP_BR = 0, /* branch */
  OP_ADD,    /* add  */
  OP_LD,     /* load */
  OP_ST,     /* store */
  OP_JSR,    /* jump register */
  OP_AND,    /* bitwise and */
  OP_LDR,    /* load register */
  OP_RTI,    /* unused */
  OP_STR,    /* store register */
  OP_NOT,    /* bitwise not */
  OP_LDI,    /* load indirect */
  OP_STI,    /* store indirect */
  OP_JMP,    /* jump */
  OP_RES,    /* reserved (unused) */
  OP_LEA,    /* load effective address */
  OP_TRAP    /* execute trap */
};
enum {
  FL_POS = 1 << 0, /* P */
  FL_ZRO = 1 << 1, /* Z */
  FL_NEG = 1 << 2, /* N */
};

uint16_t main_mem[MAX_MEM];
uint16_t reg[R_COUNT];

int main(int argc, const char *argv[]) {
  // @{ Load Arguments } @{Setup}
  if (argc < 2) {
    /* show usage string */
    cout << "Image not provided\n" << endl;

    return -1;
  }

  for (int j = 1; j < argc; ++j) {
    if (!read_image(argv[j])) {
      cout << "Vevn image is not laoded" << endl;
      return -1;
    }
  }

  /* since exactly one condition flag should be set at any given time, set the Z
     flag */
  reg[R_COND] = FL_ZRO;

  /* set the PC to starting position */
  /* 0x3000 is the default */
  enum { PC_START = 0x3000 };
  reg[R_PC] = PC_START;

  int running = 1;
  while (running) {
    /* FETCH */
    uint16_t instr = mem_read(reg[R_PC]++);
    uint16_t op = instr >> 12;

    switch (op) {
    case OP_ADD:
      /* destination register (DR) */
      uint16_t r0 = (instr >> 9) & 0x7;
      /* first operand (SR1) */
      uint16_t r1 = (instr >> 6) & 0x7;
      /* whether we are in immediate mode */
      uint16_t imm_flag = (instr >> 5) & 0x1;

      if (imm_flag) {
        uint16_t imm5 = sign_extend(instr & 0x1F, 5);
        reg[r0] = reg[r1] + imm5;
      } else {
        uint16_t r2 = instr & 0x7;
        reg[r0] = reg[r1] + reg[r2];
      }

      update_flags(r0);
    case OP_AND:
      // @{ AND } break;
    case OP_NOT:
      // @{ NOT } break;
    case OP_JMP:
      // @{ JMP } break;
    case OP_JSR:
      // @{ JSR } break;
    case OP_LD:
      // @{ LD } break;
    case OP_LDI:
      // @{ LDI } break;
    case OP_LDR:
      // @{ LDR } break;
    case OP_LEA:
      // @{ LEA } break;
    case OP_ST:
      // @{ ST } break;
    case OP_STI:
      // @{ STI } break;
    case OP_STR:
      // @{ STR } break;
    case OP_TRAP:
      // @{ TRAP } break;
    case OP_RES:
    case OP_RTI:
    default:
      // @{ BAD OPCODE } break;
    }
  }
  // @{ Shutdown }
}
