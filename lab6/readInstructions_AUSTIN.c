/*----------------------------------------------------------------------
 * Lab 6 - CPE 315
 * Jenna Stephens
 * Vankat Akkinepally
 * Austin Whaley
 *----------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readInstructions_AUSTIN.h"
#include "startChecks.h"
#include "mips_asm_header.h"

//============GLOBAL VARIABLES
MIPS mem[1024];	                                                       	         /* Room for 4K bytes */

int main(int argc, char *argv[]) {
   int memp;                                                                     //Size of pulled instructions
    
   checkInputs(argc, argv);
   memp = checkFile(argv, mem, sizeof(mem));
   readInstructions(memp);
}

void readInstructions(int memp) {
   int curr_instruction;
   int imm_value;
   unsigned int opcode;
   char instruct_type;
   int i;
   

   for (i = 0; i<memp; i+=4) {	                                                 /* i contains byte offset addresses */
      curr_instruction = mem[i/4];
      printf("@PC=%08X, ", i);

      opcode = op_code(curr_instruction);		                         //Print Opcode
      instruct_type = type(opcode, curr_instruction);			         //Print instruction type

      //if R-Type
      if (instruct_type == 'r') { 
         if (func_code(curr_instruction)) {			                 //print function code
            printReg(curr_instruction, 1, 1, 1);
            shamt(curr_instruction); 			                         //print shift amount
         }
      }

      //if I-Type, print immediate value
      if (instruct_type == 'i'|| instruct_type == 'b' || instruct_type == 's') {
         printReg(curr_instruction, 1, 1, 0);
         imm_value = imm_val(curr_instruction);
      }

      //if I-Type (Branch), print branch address
      if (instruct_type == 'b') {                     
         eff_addr(i, imm_value);
      }

      //if J-Type, print Jump Address
      if (instruct_type == 'j') {
         jmp_addr(curr_instruction);
      }

      //if Store/load I-Type
      if (instruct_type == 's') {
         eff_addr_ls(curr_instruction);
      }     

      printf("\n\n");
   }

   printf("\n");

   exit(0);
}

/************************************************************************************************
  Helper functions

 ************************************************************************************************/

unsigned int op_code(MIPS ir) {
   unsigned int op_code;

   op_code = ((ir >> 26) & 0x00000003F);                                         //Mask all but opcode
   return op_code;
}

int imm_val(MIPS ir) {                                                           //I Types
   unsigned int imm_val;
   unsigned int sign_ext;
   int sign;

   imm_val = (ir & 0x0000FFFF); 
   printf("Imm=0x%04X, ", imm_val);

   sign = (imm_val & 0x00008000);
   if (sign) {
      sign_ext = (imm_val | 0xFFFF0000);
   }
   printf("signext: 0x%08X (%d)", sign_ext, (int) sign_ext);
   return imm_val;
}

void shamt(MIPS ir) {                                                            //R Types
   unsigned int shamt;

   shamt = ((ir >> 6) & 0x00000001F);    
   printf("shamt=0x%02X ", shamt);
}

int func_code(MIPS ir) {                                                         //R Types
   unsigned int func_code;

   func_code = (ir & 0x0000003F);

   switch (func_code) {
      case 0x20: printf("R Type, Function=0x%02X (add)\n", func_code); break;
      case 0x21: printf("R Type, Function=0x%02X (addu)\n", func_code); break;
      case 0x22: printf("R Type, Function=0x%02X (sub)\n", func_code); break;
      case 0x23: printf("R Type, Function=0x%02X (subu)\n", func_code); break;
      case 0x24: printf("R Type, Function=0x%02X (and)\n", func_code); break;
      case 0x27: printf("R Type, Function=0x%02X (nor)\n", func_code); break;
      case 0x25: printf("R Type, Function=0x%02X (or)\n", func_code); break;
      case 0x26: printf("R Type, Function=0x%02X (xor)\n", func_code); break;
      case 0x00: printf("R Type, Function=0x%02X (sll)\n", func_code); break;
      case 0x02: printf("R Type, Function=0x%02X (srl)\n", func_code); break;
      case 0x03: printf("R Type, Function=0x%02X (sra)\n", func_code); break;
      case 0x04: printf("R Type, Function=0x%02X (sllv)\n", func_code); break;
      case 0x06: printf("R Type, Function=0x%02X (srlv)\n", func_code); break;
      case 0x07: printf("R Type, Function=0x%02X (srav)\n", func_code); break;
      case 0x2A: printf("R Type, Function=0x%02X (slt)\n", func_code); break;
      case 0x2B: printf("R Type, Function=0x%02X (sltu)\n", func_code); break;
      case 0x08: printf("R Type, Function=0x%02X (jr)\n", func_code); break;
      case 0x09: printf("R Type, Function=0x%02X (jalr)\n", func_code); break;
      case 0x1A: printf("R Type, Function=0x%02X (div)\n", func_code); break;
      case 0x1B: printf("R Type, Function=0x%02X (divu)\n", func_code); break;
      case 0x18: printf("R Type, Function=0x%02X (mult)\n", func_code); break;
      case 0x19: printf("R Type, Function=0x%02X (multu)\n", func_code); break;
      default:
                 printf("0x%08X - Invalid Instruction.", ir);
                 return 0;
   }
   return 1;
}

void eff_addr(int pc, int imm_value) {                                           //I Types, Branches only
   int eff_addr;

   eff_addr = pc + imm_value;
   printf(", BranchAddr=0x%08X ", eff_addr);
}

void jmp_addr(MIPS ir) {                                                         //J Types
   unsigned int jmp_addr;

   jmp_addr = (ir & 0x03FFFFFF);
   printf("addr=0x%06X \n", jmp_addr);

   jmp_addr <<= 2;	
   printf("JumpAddr=0x%08X ", jmp_addr);
}

char type(unsigned int op_code, MIPS ir) {
   switch(op_code) {
      case 0x00: return 'r';
      case 0x02: printf("OpCode=0x%02X, J Type (j), ", op_code); return 'j';
      case 0x03: printf("OpCode=0x%02X, J Type (jal), ", op_code); return 'j';
      case 0x08: printf("OpCode=0x%02X, I Type (addi)\n", op_code); return 'i';
      case 0x09: printf("OpCode=0x%02X, I Type (addiu)\n", op_code); return 'i';
      case 0x0C: printf("OpCode=0x%02X, I Type (andi)\n", op_code); return 'i';
      case 0x0D: printf("OpCode=0x%02X, I Type (ori)\n", op_code); return 'i';
      case 0x0E: printf("OpCode=0x%02X, I Type (xori)\n", op_code); return 'i';
      case 0x0A: printf("OpCode=0x%02X, I Type (slti)\n", op_code); return 'i';
      case 0x0B: printf("OpCode=0x%02X, I Type (sltiu)\n", op_code); return 'i';
      case 0x04: printf("OpCode=0x%02X, I Type (beq)\n", op_code); return 'b';
      case 0x05: printf("OpCode=0x%02X, I Type (bne)\n", op_code); return 'b';
      case 0x20: printf("OpCode=0x%02X, I Type (lb)\n", op_code); return 's';
      case 0x24: printf("OpCode=0x%02X, I Type (lbu)\n", op_code); return 's';
      case 0x21: printf("OpCode=0x%02X, I Type (lh)\n", op_code); return 's';
      case 0x25: printf("OpCode=0x%02X, I Type (lhu)\n", op_code); return 's';
      case 0x0F: printf("OpCode=0x%02X, I Type (lui)\n", op_code); return 's';
      case 0x23: printf("OpCode=0x%02X, I Type (lw)\n", op_code); return 's';
      case 0x28: printf("OpCode=0x%02X, I Type (sb)\n", op_code); return 's';
      case 0x29: printf("OpCode=0x%02X, I Type (sh)\n", op_code); return 's';
      case 0x2B: printf("OpCode=0x%02X, I Type (sw)\n", op_code); return 's';
      case 0x31: printf("OpCode=0x%02X, I Type (lwcl)\n", op_code); return 's';
      case 0x35: printf("OpCode=0x%02X, I Type (ldcl)\n", op_code); return 's';
      case 0x39: printf("OpCode=0x%02X, I Type (swcl)\n", op_code); return 's';
      case 0x3D: printf("OpCode=0x%02X, I Type (sdcl)\n", op_code); return 's';
      default:
                 printf("0x%08X - Invalid Instruction.\n", ir);
                 return '0';
   }
}


void eff_addr_ls(MIPS ir) {
   unsigned int ext_imm = (ir & 0x0000FFFF);
   ext_imm = ((ext_imm & 0x8000) ? 0xFFFF0000 : 0);
   printf(", \nEffAddr=R[");
   printRegRS(ir);
   printf("] + 0x%08X", ext_imm);
}

void printRegRS(MIPS ir) {
   int rs = (ir >> 21) & 0x1F;

   int RsReg;
   if (rs == 0) {
      printf("$zero", rs);
   } else if (rs <= 3) {
      RsReg= rs - 2;
      printf("$v%d", rs,RsReg);
   } else if (rs <= 7) {
      RsReg= rs - 4;
      printf("$a%d, ", rs,RsReg);
   } else if (rs <= 15) {
      RsReg= rs - 8;
      printf("$t%d", rs,RsReg);
   } else if (rs <= 23) {
      RsReg= rs - 16;
      printf("$s%d", rs,RsReg);
   } else if (rs == 31) {
      printf("$ra", rs);
   }

}

void printReg(MIPS ir, int rs_en, int rt_en, int rd_en) {
   int rs = (ir >> 21) & 0x1F;
   int rt = (ir >> 16) & 0x1F;
   int rd = (ir >> 11) & 0x1F;

   int RsReg;
   if (rs_en) {
      if (rs == 0) {
         printf("rs=%d ($zero), ", rs);
      } else if (rs <= 3) {
         RsReg= rs - 2;
         printf("rs=%d ($v%d), ", rs,RsReg);
      } else if (rs <= 7) {
         RsReg= rs - 4;
         printf("rs=%d ($a%d), ", rs,RsReg);
      } else if (rs <= 15) {
         RsReg= rs - 8;
         printf("rs=%d ($t%d), ", rs,RsReg);
      } else if (rs <= 23) {
         RsReg= rs - 16;
         printf("rs=%d ($s%d), ", rs,RsReg);
      } else if (rs == 31) {
         printf("rs=%d ($ra) ", rs);
      }
   }
   int RtReg;

   if (rt_en) {
      if (rt == 0) {
         printf("rt=%d ($zero), ", rt);
      } else if (rt <= 3) {
         RtReg= rt - 2;
         printf("rt=%d ($v%d), ", rt,RtReg);
      } else if (rt <= 7) {
         RtReg= rt - 4;
         printf("rt=%d ($a%d), ", rt,RtReg);
      } else if (rt <= 15) {
         RtReg= rt - 8;
         printf("rt=%d ($t%d), ", rt,RtReg);
      } else if (rt <= 23) {
         RtReg= rt - 16;
         printf("rt=%d ($s%d), ", rt,RtReg);
      } else if (rt == 31) {
         printf("rt=%d ($ra) ", rt);
      }
   }
   int RdReg;

   if (rd_en) {
      if (rd == 0) {
         printf("rd=%d ($zero), ", rd);
      } else if (rd <= 3) {
         RdReg= rd - 2;
         printf("rd=%d ($v%d), ", rd,RdReg);
      } else if (rd <= 7) {
         RdReg= rd - 4;
         printf("rd=%d ($a%d), ", rd,RdReg);
      } else if (rd <= 15) {
         RdReg= rd - 8;
         printf("rd=%d ($t%d), ", rd,RdReg);
      } else if (rd <= 23) {
         RdReg= rd - 16;
         printf("rd=%d ($s%d), ", rd,RdReg);
      } else if (rd == 31) {
         printf("rd=%d ($ra) ", rd);
      }
   }
}

void printValues(INST instruction) {                                             // Prints the instruction values
   switch (instruction.type) {
      case ('r') : printf("values\n"); break;
      case ('i') : break;
      case ('b') : break;
      case ('s') : break;
      case ('j') : break;
   }
}
