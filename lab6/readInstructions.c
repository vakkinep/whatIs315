/*----------------------------------------------------------------------
 * Lab 6 - CPE 315
 * Jenna Stephens
 * Vankat Akkinepally
 * Austin Whaley
 *----------------------------------------------------------------------*/

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
void execute(INST instruction, int ir);


/*int main(int argc, char *argv[]) {
  int memp;                                                                     //Size of pulled instructions
  REG regs[32] = {0};

  checkInputs(argc, argv);
  memp = checkFile(argv, mem, sizeof(mem));
  readInstructions(memp, regs);
  printValues(instruct);
  }*/

int readInstructions(int memp, REG regs[], int pc, INST instruct) {
   while(pc < memp) {	                                       /* While the pc is less than total instructions*/
      pc = readNextInst(memp, regs, pc, instruct);								//increment pc inside for jump and branch
   }
   printf("\n\n");
   return pc;
}

int readNextInst(int memp, REG regs[], int pc, INST instruct) {
   int curr_instruction = mem[pc/4];
   printf("\n@PC=%08X, ", pc);

   instruct.opcode = op_code(curr_instruction);		                         //Print Opcode
   instruct.type   = type(instruct.opcode, curr_instruction);			         //Print instruction type

   //if R-Type
   if (instruct.type == 'r') { 
      instruct.func_code = func_code(curr_instruction);
      if (instruct.func_code) {			                 					//print function code
         instruct.rs = reg_s(curr_instruction);
         instruct.rt = reg_t(curr_instruction);
         instruct.rd = reg_d(curr_instruction);
         instruct.shamt = shamt(curr_instruction); 			                         //print shift amount
         execute(instruct, curr_instruction);
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
      regs[31] = pc + 4;
      return (pc = eff_addr(pc, instruct.immed));
   }

   //if J-Type, print Jump Address
   if (instruct.type == 'j') {
      instruct.jmp_addr = jmp_addr(curr_instruction);
      if (instruct.opcode == 0x03) {												//jump and link
         regs[31] = pc + 4;
      }
      return (pc = instruct.jmp_addr); 
   }

   //if Store/load I-Type
   if (instruct.type == 's') {
      eff_addr_ls(curr_instruction);
   }     

   return pc+4;																	//increment pc
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

   sign = (imm_val & 0x00008000);
   if (sign) {
      sign_ext = (imm_val | 0xFFFF0000);
   }
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

   if (func_code == 0x20 || func_code == 0x21 || func_code == 0x22 || func_code == 0x23 || func_code == 0x24 || func_code == 0x27 || func_code == 0x25 || func_code == 0x26 || func_code == 0x00 || func_code == 0x02 || func_code == 0x03 || func_code == 0x04 || func_code == 0x06 || func_code == 0x07 || func_code == 0x2A || func_code == 0x2B || func_code == 0x08 || func_code == 0x09 || func_code == 0x1A || func_code == 0x1B || func_code == 0x18 || func_code == 0x19) {
      return func_code;
   }
   else {
      return 0;
   }
}

void execute(INST instruct, int ir) {
   switch (instruct.func_code) {
      case 0x20: break; 	// (add)
      case 0x21: break; 	// (addu)
      case 0x22: break; 	// (sub)
      case 0x23: break; 	// (subu)
      case 0x24: break; 	// (and)
      case 0x27: break; 	// (nor)
      case 0x25: break; 	// (or)
      case 0x26: break; 	// (xor)
      case 0x00: break; 	// (sll)
      case 0x02: break; 	// (srl)
      case 0x03: break; 	// (sra)
      case 0x04: break; 	// (sllv)
      case 0x06: break; 	// (srlv)
      case 0x07: break; 	// (srav)
      case 0x2A: break; 	// (slt)
      case 0x2B: break; 	// (sltu)
      case 0x08: break; 	// (jr)
      case 0x09: break; 	// (jalr)
      case 0x1A: break; 	// (div)
      case 0x1B: break; 	// (divu)
      case 0x18: break; 	// (mult)
      case 0x19: break; 	// (multu)
      default:
                 printf("FUNCTION 0x%08X - Invalid Instruction.", ir);
   }
}

int eff_addr(int pc, int imm_value) {                                           //I Types, Branches only
   int eff_addr;

   eff_addr = pc + imm_value;
   return eff_addr;
}

unsigned int jmp_addr(MIPS ir) {                                                         //J Types
   unsigned int jmp_addr;

   jmp_addr = (ir & 0x03FFFFFF);

   jmp_addr <<= 2;	
   return jmp_addr;
}

char type(unsigned int op_code, MIPS ir) {
   if (ir == 0) {                      //nop check
      return 'n';
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
      case 0x31: return 's';		// I Type (lwcl) 
      case 0x35: return 's';		// I Type (ldcl) 
      case 0x39: return 's';		// I Type (swcl) 
      case 0x3D: return 's';		// I Type (sdcl) 
      default:
                 printf("0x%08X - Invalid Instruction.\n", ir);
                 return 'x';
   }
}


unsigned int eff_addr_ls(MIPS ir) {
   unsigned int ext_imm = (ir & 0x0000FFFF);
   ext_imm |= ((ext_imm & 0x8000) ? 0xFFFF0000 : 0);
   reg_s(ir);
   return ext_imm;
}

int reg_s(MIPS ir) {
   int rs = (ir >> 21) & 0x1F;
   return rs;
}

int reg_t(MIPS ir) {
   int rt = (ir >> 16) & 0x1F;
   return rt;
}

int reg_d(MIPS ir) {
   int rd = (ir >> 11) & 0x1F;
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
