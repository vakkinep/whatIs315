/*----------------------------------------------------------------------
 * Lab 6 - CPE 315
 * Jenna Stephens
 * Vankat Akkinepally
 * Austin Whaley
 *----------------------------------------------------------------------*/

typedef struct INSTRUCTION {
   char type;
   unsigned int  opcode;
   unsigned int  rs;
   unsigned int  rt;
   unsigned int  rd;
   unsigned int  shamt;
   unsigned int  func_code;
   int  immed;
   unsigned int  jmp_addr;
} INST;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readInstructions.h"
#include "startChecks.h"
#include "mips_asm_header.h"

//============GLOBAL VARIABLES
MIPS mem[1024];	                                                       	         /* Room for 4K bytes */


//============PROTOTYPE
void printValues(INST instruction);


int main(int argc, char *argv[]) {
   int memp;                                                                     //Size of pulled instructions
   REG regs[32] = {0};

   checkInputs(argc, argv);
   memp = checkFile(argv, mem, sizeof(mem));
   readInstructions(memp, regs);
}

void readInstructions(int memp, REG regs[]) {
   int curr_instruction;
   INST instruct;
   int pc;


   for (pc = 0; pc<memp; pc+=4) {	                                                 /* i contains byte offset addresses */
      curr_instruction = mem[pc/4];
      printf("\n@PC=%08X, ", pc);

      instruct.opcode = op_code(curr_instruction);		                         //Print Opcode
      instruct.type = type(instruct.opcode, curr_instruction);			         //Print instruction type

      //if R-Type
      if (instruct.type == 'r') { 
         instruct.func_code = func_code(curr_instruction);
         if (instruct.func_code) {			                 					//print function code
            instruct.rs = reg_s(curr_instruction);
            instruct.rt = reg_t(curr_instruction);
            instruct.rd = reg_d(curr_instruction);
            instruct.shamt = shamt(curr_instruction); 			                         //print shift amount
         }
      }

      //if I-Type, print immediate value
      if (instruct.type == 'i'|| instruct.type == 'b' || instruct.type == 's') {
         instruct.rs = reg_s(curr_instruction);
         instruct.rt = reg_t(curr_instruction);
         instruct.immed = imm_val(curr_instruction);
      }

      //if I-Type (Branch), print branch address
      if (instruct.type == 'b') {
         eff_addr(pc, instruct.immed);
      }

      //if J-Type, print Jump Address
      if (instruct.type == 'j') {
         instruct.jmp_addr = jmp_addr(curr_instruction);
         if (instruct.opcode == 0x03) {												//jump and link
         	regs[31] = pc;
         }
         pc = instruct.jmp_addr; 
      }

      //if Store/load I-Type
      if (instruct.type == 's') {
         eff_addr_ls(curr_instruction);
      }     

      printValues(instruct);
   }


   printf("\n\n");
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

unsigned int shamt(MIPS ir) {                                                            //R Types
   unsigned int shamt;

   shamt = ((ir >> 6) & 0x00000001F);    
   return shamt;
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

int eff_addr(int pc, int imm_value) {                                           //I Types, Branches only
   int eff_addr;

   eff_addr = pc + imm_value;
   return eff_addr;
}

unsigned int jmp_addr(MIPS ir) {                                                         //J Types
   unsigned int jmp_addr;

   jmp_addr = (ir & 0x03FFFFFF);
   //printf("addr=0x%06X \n", jmp_addr);

   jmp_addr <<= 2;	
   return jmp_addr;
}

char type(unsigned int op_code, MIPS ir) {
   switch(op_code) {
      case 0x00: return 'n'; //nop
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


unsigned int eff_addr_ls(MIPS ir) {
   unsigned int ext_imm = (ir & 0x0000FFFF);
   ext_imm |= ((ext_imm & 0x8000) ? 0xFFFF0000 : 0);
   //printf(", \nEffAddr=R[");
   reg_s(ir);
   //printf("] + 0x%08X", ext_imm);
   return ext_imm;
}

int reg_s(MIPS ir) {
   int rs = (ir >> 21) & 0x1F;
   printReg(rs, "rs");
   return rs;
}

int reg_t(MIPS ir) {
   int rt = (ir >> 16) & 0x1F;
   printReg(rt, "rt");
   return rt;
}

int reg_d(MIPS ir) {
   int rd = (ir >> 11) & 0x1F;
   printReg(rd, "rd");
   return rd;
}

void printReg(int reg, char* title) { //title will be which register (rs, rt, rd)
    int name;
  if (reg == 0) {
     printf("%s=%d ($zero), ", title, reg);
  } else if (reg <= 3) {
     name= reg - 2;
     printf("%s=%d ($v%d), ", title, reg, name);
  } else if (reg <= 7) {
     name= reg - 4;
     printf("%s=%d ($a%d), ", title, reg, name);
  } else if (reg <= 15) {
     name= reg - 8;
     printf("%s=%d ($t%d), ", title, reg, name);
  } else if (reg <= 23) {
     name= reg - 16;
     printf("%s=%d ($s%d), ", title, reg, name);
  } else if (reg == 31) {
     printf("%s=%d ($ra, ", title, reg);
  }
}

void printValues(INST instruction) {                                             // Prints the instruction values
   printf("OPCODE: 0x%06X\tTYPE: %c\n",instruction.opcode, instruction.type);
   switch (instruction.type) {
      case ('r') : 
         printf("values: \n");                                                   //r-type       AUSTIN
         printReg(instruction.rs, "rs");
         printReg(instruction.rt, "rt");
         printReg(instruction.rd, "rd");
         printf("\n");
         printf("SHAMT: %d\t FUNCTION: 0x%02X\n\n\n", instruction.shamt, instruction.func_code);
         break;
      case ('i') : break;                                                        //i-type       AUSTIN
         printf("values: \n");
         printReg(instruction.rs, "rs");
         printReg(instruction.rt, "rt");
         printf("Immed value: 0x%04X\n", instruction.immed);
      case ('b') : break;                                                        //brn inst     VINNIE
      case ('s') : break;                                                        //ld/store     VINNIE
      case ('j') : printf("jmp_addr=0x%06X\n", jmp_addr); break;                                                        //j-type
      case ('n') : break;                                                        //nop
   }
}
