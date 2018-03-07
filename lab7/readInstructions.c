/*----------------------------------------------------------------------
 * Lab 7 (MIPS Pipeline Simulator) - CPE 315
 * Jenna Stephens
 * Venkat Akkinepally
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
unsigned int heap[4096];

int readInstructions(int memp, REG regs[], int pc, INST instruct, int* inst_ran, int* clocks, int* mem_ref) {
   while(pc < memp) {	                                       /* While the pc is less than total instructions*/
      pc = readNextInst(memp, regs, pc, instruct, inst_ran, clocks, mem_ref);								//increment pc inside for jump and branch
   }
   printf("\n\n");
   return pc;
}

int readNextInst(int memp, REG regs[], int pc, INST instruct, int* inst_ran, int* clocks, int* mem_ref) {
   int curr_instruction = mem[pc/4];
   (*inst_ran)++;
   instruct.opcode = op_code(curr_instruction);		    
   instruct.type   = type(instruct.opcode, curr_instruction);

   //if R-Type
   if (instruct.type == 'r') { 
      instruct.func_code = func_code(curr_instruction);
      if (instruct.func_code) {		
         instruct.rs = reg_s(curr_instruction);
         instruct.rt = reg_t(curr_instruction);
         instruct.rd = reg_d(curr_instruction);
         instruct.shamt = shamt(curr_instruction);
         (*clocks) += 4;
      }

      if (instruct.func_code == 0x08 | instruct.func_code == 0x09) {
         instruct.jmp_addr = regs[instruct.rs];
         return execute_jump(instruct, regs, pc, clocks);
      }
   }

   //if I-Type, print immediate value
   if (instruct.type == 'i'|| instruct.type == 'b' || instruct.type == 's') {
      (*clocks) += 4;
      instruct.rs = reg_s(curr_instruction);
      instruct.rt = reg_t(curr_instruction);
      instruct.immed = imm_val(curr_instruction);
   }

   //if I-Type (Branch), print branch address
   if (instruct.type == 'b') {
      (*clocks) -= 1;
      return execute_branch(instruct, regs, pc);
   }

   //if J-Type, print Jump Address
   if (instruct.type == 'j') {
      instruct.jmp_addr = jmp_addr(curr_instruction);

      return execute_jump(instruct, regs, pc, clocks);
   }

   //if Store/load I-Type
   if (instruct.type == 's') {
      (*mem_ref)++;
      if (instruct.opcode == 0x28 || instruct.opcode == 0x28 || instruct.opcode == 0x2B) {
         (*clocks) -= 1;
      }
      instruct.eff_addr = eff_addr_load(curr_instruction);
   }     


   if (instruct.func_code && instruct.type != 'x') {        //if its a valid command and changes things
      execute(instruct, regs);
   }

   if (instruct.type == 'n') {
      (*clocks) += 3;
   }

   if (curr_instruction == 0x0000000C) {
      if (syscall(regs)) {
         return memp;
      }
   }
   
   return pc+4;
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
   return imm_val;
}

int sign_ext(INST instruction) {
   int sign;
   unsigned int sign_ext;

   sign_ext = instruction.immed;
   sign = (instruction.immed & 0x00008000);

   if (sign) {
      sign_ext |= 0xFFFF0000; 
   }

   return sign_ext;
}

unsigned int shamt(MIPS ir) {                                                            //R Types
   unsigned int shamt;

   shamt = ((ir >> 6) & 0x00000001F);    
   return shamt;
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

int execute_jump(INST instruct, REG regs[], int pc, int* clocks) {
   if (instruct.opcode == 0x03 | instruct.func_code == 0x09) {      //if jal/jalr
      regs[31] = pc + 4;
      (*clocks) += 1;
   }
   (*clocks) -= 1;
   return (pc = instruct.jmp_addr);
}

int execute_branch(INST instruct, REG regs[], int pc) {
   if (instruct.opcode == 0x04 && instruct.rs == instruct.rt) { //beq
      instruct.brn_addr = pc + eff_addr(pc, instruct.immed);
      return (pc = instruct.brn_addr);
   }
   else if (instruct.opcode == 0x05 && instruct.rs != instruct.rt) { //bne
      instruct.brn_addr = pc + eff_addr(pc, instruct.immed);
      return (pc = instruct.brn_addr);
   }
   return pc+4;
}

void execute(INST instruct, REG regs[]) {
   switch(instruct.type) {
      case('r'): 
         switch (instruct.func_code) {
            case 0x20: 
               regs[instruct.rd] = ((int)regs[instruct.rs]) + ((int)regs[instruct.rt]);
               break; 	// (add)
            case 0x21:
               regs[instruct.rd] = ((unsigned int)regs[instruct.rs]) + ((unsigned int)regs[instruct.rt]);
               break; 	// (addu)
            case 0x22:  
               regs[instruct.rd] = ((int)regs[instruct.rs]) - ((int)regs[instruct.rt]);
               break; 	// (sub)
            case 0x23:  
               regs[instruct.rd] = ((unsigned int)regs[instruct.rs]) - ((unsigned int)regs[instruct.rt]);
               break; 	// (subu)
            case 0x24:  
               regs[instruct.rd] = ((unsigned int)regs[instruct.rs]) & ((unsigned int)regs[instruct.rt]);
               break; 	// (and)
            case 0x27: 
               regs[instruct.rd] = !(((unsigned int)regs[instruct.rs]) | ((unsigned int)regs[instruct.rt]));
               break; 	// (nor)
            case 0x25: 
               regs[instruct.rd] = (((unsigned int)regs[instruct.rs]) | ((unsigned int)regs[instruct.rt]));
               break; 	// (or)
            case 0x26: 
               regs[instruct.rd] = (((unsigned int)regs[instruct.rs]) ^ ((unsigned int)regs[instruct.rt]));
               break; 	// (xor)
            case 0x00: 
               regs[instruct.rd] = ((unsigned int)regs[instruct.rt]) << instruct.shamt;
               break; 	// (sll)
            case 0x02:  
               regs[instruct.rd] = ((unsigned int)regs[instruct.rt]) >> instruct.shamt;
               break; 	// (srl)
            case 0x03:  
               regs[instruct.rd] = ((int)regs[instruct.rt]) >> instruct.shamt;
               break; 	// (sra)
            case 0x04: 
               regs[instruct.rd] = ((unsigned int)regs[instruct.rt]) << ((unsigned int)regs[instruct.rs]); 
               break; 	// (sllv)
            case 0x06: 
               regs[instruct.rd] = ((unsigned int)regs[instruct.rt]) >> ((unsigned int)regs[instruct.rs]); 
               break; 	// (srlv)
            case 0x07: 
               regs[instruct.rd] = ((int)regs[instruct.rt]) >> ((int)regs[instruct.rs]); 
               break; 	// (srav)
            case 0x2A: 
               regs[instruct.rd] = (((int)regs[instruct.rs]) < ((int)regs[instruct.rt]))? 1:0; 
               break; 	// (slt)
            case 0x2B: 
               regs[instruct.rd] = (((unsigned int)regs[instruct.rs]) < ((unsigned int)regs[instruct.rt]))? 1:0; 
               break; 	// (sltu)
         }
         break;

      case('i') :
         switch (instruct.opcode) {
            case 0x08:  regs[instruct.rt] = ((int) regs[instruct.rs])  + ((int) instruct.immed);  break;	 // I Type (addi) 
            case 0x09:	
                        regs[instruct.rt] = ((unsigned int) regs[instruct.rs]) + ((unsigned int) instruct.immed); break;   // Type (addiu) 
            case 0x0C:  
                        regs[instruct.rt] = ((unsigned int) regs[instruct.rs]) & ((unsigned int) instruct.immed); break; 	 // I Type (andi) 
            case 0x0D:	
                        regs[instruct.rt] = ((unsigned int) regs[instruct.rs]) | ((unsigned int) instruct.immed);break;   // I Type (ori) 
            case 0x0E:	
                        regs[instruct.rt] = ((unsigned int) regs[instruct.rs]) ^ ((unsigned int) instruct.immed);break;	 // I Type (xori) 
            case 0x0A:	
                        regs[instruct.rt] =  (((int) regs[instruct.rs]) < ((int) instruct.immed));break;   // I Type (slti) 
            case 0x0B:	
                        regs[instruct.rt] =  (((unsigned int) regs[instruct.rs]) < ((unsigned int) instruct.immed)); break;	 // I Type (sltiu) 
         }
         break;

      case('s') :
         switch (instruct.opcode) {
            case 0x20:				// I Type (lb) 
               regs[instruct.rt] = heap[regs[instruct.rs] + instruct.eff_addr] & 0x000000FF;
               break;
            case 0x24:				// I Type (lbu) 
               regs[instruct.rt] = heap[(unsigned int)regs[instruct.rs] + (unsigned int)instruct.eff_addr] & 0x000000FF;
               break;
            case 0x21:				// I Type (lh)
               regs[instruct.rt] = heap[regs[instruct.rs] + instruct.eff_addr] & 0x0000FFFF;
               break;
            case 0x25:				// I Type (lhu)
               regs[instruct.rt] = heap[(unsigned int)regs[instruct.rs] + (unsigned int)instruct.eff_addr] & 0x0000FFFF;
               break; 
            case 0x0F:				// I Type (lui)
               regs[instruct.rt] = heap[regs[instruct.rs] + instruct.eff_addr] & 0xFFFF0000;
               break;
            case 0x23: break;		// I Type (lw)
                       regs[instruct.rt] = heap[regs[instruct.rs] + instruct.eff_addr];
                       break;
            case 0x28:				// I Type (sb) 
                       heap[instruct.rs + instruct.eff_addr] = regs[instruct.rt] & 0x000000FF;
                       break;
            case 0x29: break;		// I Type (sh) 
                       heap[instruct.rs + instruct.eff_addr] = regs[instruct.rt] & 0x0000FFFF;
                       break;
            case 0x2B: break;		// I Type (sw) 
                       heap[instruct.rs + instruct.eff_addr] = regs[instruct.rt];
                       break; 
         }
         break;


      case('b') : 
         switch (instruct.opcode) {
            case 0x04: break;		// I Type (beq) 
            case 0x05: break;		// I Type (bne) 
         }
         break;

      case('j') : //do nothing for jump its already done too
         break;   
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
      default:
                 printf("0x%08X - Invalid Instruction.\n", ir);
                 return 'x';
   }
}


unsigned int eff_addr_load(MIPS ir) {
   unsigned int ext_imm = (ir & 0x0000FFFF);
   ext_imm |= ((ext_imm & 0x8000) ? 0xFFFF0000 : 0);
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
      printf("%s=%d ($ra), ", title, reg);
   }
}

void printValues(INST instruction) {                                             // Prints the instruction values
   printf("OPCODE: 0x%02X\tTYPE: %c\n",instruction.opcode, instruction.type);
   switch (instruction.type) {
      case ('r') : 
         printf("values: \n");                                 
         printReg(instruction.rs, "rs");
         printReg(instruction.rt, "rt");
         printReg(instruction.rd, "rd");
         printf("\n");
         printf("SHAMT: %d\t FUNCTION: 0x%02X\n", instruction.shamt, instruction.func_code);
         break;
      case ('i') :                                            
         printf("values: \n");
         printReg(instruction.rs, "rs");
         printReg(instruction.rt, "rt");
         printf("Immed value: 0x%04X\n", instruction.immed);
         break;
      case ('b') : 
         printf("values: \nBranch Address: 0x%08X\n", instruction.brn_addr);
         break;                                              
      case ('s') : 
         printf("values: \n");
         printReg(instruction.rs, "rs");
         printReg(instruction.rt, "rt");
         printf("Immed value: 0x%04X\t", instruction.immed);
         printf("Sign Ext: 0x%08X\n", sign_ext(instruction));
         printf("EffAddr=R[ ");
         printReg(instruction.rs, "rs");
         printf("] + 0x%08X\n",sign_ext(instruction));
         break;                                             
      case ('j') : printf("jmp_addr=0x%06X\n", instruction.jmp_addr); break;
      case ('n') : break;                                  
   }
}

int syscall(REG* regs) {
   if (regs[2] == 10) {
      return 1; 
   }
   if (regs[2] == 1) {
      printf("%d\n", regs[4]);   
   }
   return 0;
}
