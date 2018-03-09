#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pipelined.h"

INST fetch(INST instruct, MIPS mem[]) {
   instruct.curr_instruction = mem[instruct.pc];    // grab current instruction from prog_rom
   instruct.pc += 4;                                // increment pc
   return instruct;
}

//=============================================================================================

INST decode(INST instruct, REG regs[]) {
   instruct.opcode = op_code(instruct.curr_instruction);
   instruct.type = type(instruct.opcode, instruct.curr_instruction);

   //if R-Type
   if (instruct.type == 'r') { 
      instruct.func_code = func_code(instruct.curr_instruction);
      if (instruct.func_code) {		
         instruct.rs = reg_s(instruct.curr_instruction);
         instruct.rs_value = regs[instruct.rs];

         instruct.rt = reg_t(instruct.curr_instruction);
         instruct.rt_value = regs[instruct.rt];

         instruct.rd = reg_d(instruct.curr_instruction);
         instruct.rd_value = regs[instruct.rd];

         instruct.shamt = shamt(instruct.curr_instruction);
      }
      //jr/jalr
      if (instruct.func_code == 0x08 | instruct.func_code == 0x09) {
         instruct.jmp_addr = regs[instruct.rs];
      }
   }

   //if I-Type
   if (instruct.type == 'i'|| instruct.type == 'b' || instruct.type == 's') {

      instruct.rs = reg_s(instruct.curr_instruction);
      instruct.rs_value = regs[instruct.rs]; 

      instruct.rt = reg_t(instruct.curr_instruction);
      instruct.rt_value = regs[instruct.rt];

      instruct.immed = imm_val(instruct.curr_instruction);
      instruct.sign_ext = sign_extend(instruct);
   }

   if (instruct.type = 'y') {
      instruct.rs = 2;
      instruct.rs_value = regs[instruct.rs]; 
   }
}

//================== HELPERS ========================================================================

unsigned int op_code(MIPS ir) {
   return ((ir >> 26) & 0x00000003F);                                         //Mask all but opcode
}

char type(unsigned int op_code, MIPS ir) {
   if (ir == 0) {                      //nop check
      return 'n';
   }
   if (ir == 0x0000000C) {
      return 'y';
   }
   switch(op_code) {
      case 0x00: return 'r'; 		// R Type
      case 0x02: return 'j'; 		// J Type (j), 
      case 0x03: return 'j';		// J Type (jal) 
      case 0x08: return 'i';		// I Type (addi) 
      case 0x09: return 'i';		// Type (addiu) 
      case 0x0C: return 'i';		// I Type (andi) 
      case 0x0D: return 'i';		// I Type (ori) 
      case 0x0E: return 'i';		// I Type (xori) 
      case 0x0A: return 'i';		// I Type (slti) 
      case 0x0B: return 'i';		// Type (sltiu) 
      case 0x04: return 'b';		// I Type (beq) 
      case 0x05: return 'b';		// I Type (bne) 
      case 0x20: return 's';		// I Type (lb) 
      case 0x24: return 's';		// I Type (lbu) 
      case 0x21: return 's';		// I Type (lh) 
      case 0x25: return 's';		// I Type (lhu) 
      case 0x0F: return 's';		// I Type (lui) 
      case 0x23: return 's';		// I Type (lw) 
      case 0x28: return 's';		// I Type (sb) 
      case 0x29: return 's';		// I Type (sh) 
      case 0x2B: return 's';		// I Type (sw)  
      default:
                 printf("0x%08X - Invalid Instruction.\n", ir);
                 return 'x';
   }
}

int func_code(MIPS ir) {                                                         //R Types
   unsigned int func_code;

   func_code = (ir & 0x0000003F);

   if (func_code == 0x0C || func_code == 0x20 || func_code == 0x21 || func_code == 0x22 || func_code == 0x23 || func_code == 0x24 || func_code == 0x27 || func_code == 0x25 || func_code == 0x26 || func_code == 0x00 || func_code == 0x02 || func_code == 0x03 || func_code == 0x04 || func_code == 0x06 || func_code == 0x07 || func_code == 0x2A || func_code == 0x2B || func_code == 0x08 || func_code == 0x09 || func_code == 0x1A || func_code == 0x1B || func_code == 0x18 || func_code == 0x19) {
      return func_code;
   }
   else {
      printf("0x%08X - Invalid Instruction.\n", ir);
      return 0;
   }
}

int reg_s(MIPS ir) {
   return (ir >> 21) & 0x1F;
}

int reg_t(MIPS ir) {
   return (ir >> 16) & 0x1F;
}

int reg_d(MIPS ir) {
   return (ir >> 11) & 0x1F;
}

unsigned int shamt(MIPS ir) {                                                            //R Types
   return ((ir >> 6) & 0x00000001F);    
}

int imm_val(MIPS ir) {                                                           //I Types
   return (ir & 0x0000FFFF); 
}

int sign_extend(INST instruction) {
   int sign;
   unsigned int sign_ext;

   sign_ext = instruction.immed;
   sign = (instruction.immed & 0x00008000);

   if (sign) {
      sign_ext |= 0xFFFF0000; 
   }

   return sign_ext;
}
//=============================================================================================================
//=============================================================================================================
