#include "lc3.h"
#include "cases.h"
#include <cstdint>
#include <iostream>
#include <sys/types.h>
using namespace std;

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
    uint16_t r0;
    uint16_t r1;
    uint16_t imm_flag;
    uint16_t pc_offset;
    uint16_t base_r;

    switch (op) {
    case OP_ADD: {
      /* destination register (DR) */
      r0 = (instr >> 9) & 0x7;
      /* first operand (SR1) */
      r1 = (instr >> 6) & 0x7;
      /* whether we are in immediate mode */
      imm_flag = (instr >> 5) & 0x1;

      if (imm_flag) {
        uint16_t imm5 = sign_extend(instr & 0x1F, 5);
        reg[r0] = reg[r1] + imm5;
      } else {
        uint16_t r2 = instr & 0x7;
        reg[r0] = reg[r1] + reg[r2];
      }

      update_flags(r0);
      break;
    }
    case OP_AND: {
      r0 = (instr >> 9) & 0x7;
      /* first operand (SR1) */
      r1 = (instr >> 6) & 0x7;
      imm_flag = (instr >> 5) & 0x1;

      if (imm_flag) {
        uint16_t imm5 = sign_extend(instr & 0x1F, 5);
        reg[r0] = reg[r1] & imm5;
      } else {
        uint16_t r2 = instr & 0x7;
        reg[r0] = reg[r1] & reg[r2];
      }
      update_flags(r0);
      break;
    }

    case OP_NOT: {
      r0 = (instr >> 9) & 0x7;
      /* first operand (SR1) */
      r1 = (instr >> 6) & 0x7;

      uint16_t compliment = -reg[r1];
      reg[r0] = compliment;
      update_flags(r0);
      break;
    }
      // @{ NOT } break;
    case OP_JMP: {
      base_r = (instr >> 6) & 0x7;
      reg[R_PC] = reg[base_r];
    }
    case OP_JSR: {
      reg[R_R7] = reg[R_PC];
      uint16_t bit11 = (instr >> 11);
      if ((bit11 & 0x1) == 1) {
        pc_offset = sign_extend(bit11, 11);
        reg[R_PC] = reg[R_PC] + pc_offset;

      } else {
        base_r = (instr >> 6) & 0x7;
        reg[R_PC] = reg[base_r];
      }
      // @{ JSR } break;
    }
    case OP_LD: {
      r0 = (instr >> 9) & 0x7;
      /* PCoffset 9*/
      pc_offset = sign_extend(instr & 0x1FF, 9);
      /* add pc_offset to the current PC, look at that memory location to get
       * the final address */
      reg[r0] = mem_read(reg[R_PC] + pc_offset);
      update_flags(r0);
      break;
    }

    case OP_LDI: {

      /* destination register (DR) */

      r0 = (instr >> 9) & 0x7;
      /* PCoffset 9*/
      pc_offset = sign_extend(instr & 0x1FF, 9);
      /* add pc_offset to the current PC, look at that memory location to get
       * the final address */
      reg[r0] = mem_read(mem_read(reg[R_PC] + pc_offset));
      update_flags(r0);
      break;
    }

      // @{ LDI } break;
    case OP_LDR: {
      r0 = (instr >> 9) & 0x7;

      base_r = (instr >> 6) & 0x7;
      reg[r0] = mem_read(base_r + sign_extend(instr & 0x3F, 6));
      update_flags(r0);

      // @{ LDR } break;
    }
    case OP_LEA: {

      r0 = (instr >> 9) & 0x7;
      reg[r0] = reg[R_PC] + sign_extend(instr & 0x1FF, 9);
      reg[R_PC]++;
      update_flags(r0);
      break;

      // @{ LEA } break;
    }
    case OP_ST: {
      r0 = (instr >> 9) & 0x7;
      pc_offset = sign_extend(instr & 0x1FF, 9);
      mem_write(reg[R_PC] + pc_offset, reg[r0]);
      // @{ ST } break;
    }
    case OP_STI: {
      r0 = (instr >> 9) & 0x7;
      pc_offset = sign_extend(instr & 0x1FF, 9);
      mem_write(mem_read[reg[R_PC] + pc_offset], reg[r0]);
      // @{ STI } break;
    }
    case OP_STR:
      r0 = (instr >> 9) & 0x7;
      pc_offset = sign_extend(instr & 0x3F, 6);
      base_r = (instr >> 6) & 0x7;
      mem_write(reg[base_r] + pc_offset, reg[r0]);
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
