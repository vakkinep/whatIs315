#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pipelined.h"

//============== Global Variables ===============================================================================
MIPS mem[1024];                                      /* Room for 4K bytes */
unsigned int heap[4096];
extern int haltflag;

//============== Fetch ===============================================================================================

INST fetch(INST instruct, MIPS mem[]) {
   instruct.curr_instruction = mem[instruct.pc];    // grab current instruction from prog_rom
   instruct.pc += 4;                                // increment pc
   return instruct;
}

//================ Decode =============================================================================

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

//============= Execute ================================================================================

INST execute(INST instruct) {
   if (instruct.type == 'r') {      //R Types
      //printf("R Type\n");
      return execute_r_helper(instruct);
   }

   else if (instruct.type == 'i') {    //I Types
      //printf("I Type\n");
      return execute_i_helper(instruct); 
   }

   else if (instruct.type == 's') {    //Load and Store Types
      return execute_s_helper(instruct);
   }

   else if (instruct.type == 'b') {    //Branch Type
      //printf("Branch\n");
      return execute_b_helper(instruct);
   }

   else if (instruct.type == 'j') {    //Jump Type
      //printf("Jump\n");
      return execute_j_helper(instruct);
   }  

   else if (instruct.type == 'y') { //HALT
      //printf("Syscall\n");
      if (instruct.rs_value == 10) {
          haltflag = 1;
         return instruct;
      }
   }
   else { 
      //printf("Could not find type\n");
      return instruct;
   }
}

//=============== Memory ===================================================================================

INST memory(INST instruct) {
   if (instruct.type == 's') {      //Load and Store 
      switch (instruct.opcode) {
         case 0x20:           // I Type (lb) 
            instruct.rt_value = heap[instruct.heap_addr] & 0x000000FF;
            break;
         case 0x24:           // I Type (lbu) 
            instruct.rt_value = heap[instruct.heap_addr] & 0x000000FF;
            break;
         case 0x21:           // I Type (lh)
            instruct.rt_value = heap[instruct.heap_addr] & 0x0000FFFF;
            break;
         case 0x25:           // I Type (lhu)
            instruct.rt_value = heap[instruct.heap_addr] & 0x0000FFFF;
            break; 
         case 0x0F:           // I Type (lui)
            instruct.rt_value = heap[instruct.heap_addr] & 0xFFFF0000;
            break;
         case 0x23:           // I Type (lw)
            instruct.rt_value = heap[instruct.heap_addr];
            break;
         case 0x28:           // I Type (sb) 
            heap[instruct.heap_addr] = instruct.rt_value & 0x000000FF;
            break;
         case 0x29:          // I Type (sh) 
            heap[instruct.heap_addr] = instruct.rt_value & 0x0000FFFF;
            break;
         case 0x2B:         // I Type (sw) 
            heap[instruct.heap_addr] = instruct.rt_value;
            break; 
         }

   }

   if (instruct.type == 'b') {  //Branch
      switch (instruct.opcode) {
         case 0x04:     // I Type (beq) 
            if (instruct.rs == instruct.rt) {
               instruct.pc = instruct.brn_addr;    //set pc to branch address
            }
            break;
         case 0x05:     // I Type (bne)
            if (instruct.rs != instruct.rt) {
               instruct.pc = instruct.brn_addr;    //set pc to branch address
            }
            break;
      }

   }

   if (instruct.type == 'j') {     //Jump
         if (instruct.opcode == 0x03 || instruct.func_code == 0x09) {  // (jal, jalr)
            instruct.ra = instruct.pc;
         }
         instruct.pc = instruct.jmp_addr;
   }

   return instruct;
}

//================== Write back ==============================================================================

INST writeback(INST instruct) {
   return instruct;
}

//===========================================================================================================

//================== DECODE HELPERS ========================================================================

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

//================== Execute Helpers ===============================================================

INST execute_r_helper(INST instruct) {
   switch (instruct.func_code) {
      case 0x20:  // (add) 
         instruct.rd_value = (int)instruct.rs_value + (int)instruct.rt_value;
         break;
      case 0x21:  // (addu)
         instruct.rd_value = (unsigned int)instruct.rs_value + (unsigned int)instruct.rt_value;
         break;   
      case 0x22:  // (sub)
         instruct.rd_value = (int)instruct.rs_value - (int)instruct.rt_value;
         break;
      case 0x23:  // (subu)
         instruct.rd_value = (unsigned int)instruct.rs_value - (unsigned int)instruct.rt_value;
         break;   
      case 0x24:  // (and)
         instruct.rd_value = (unsigned int)instruct.rs_value & (unsigned int)instruct.rt_value;
         break;   
      case 0x27:  // (nor)
         instruct.rd_value = (unsigned int)instruct.rs_value | (unsigned int)instruct.rt_value;
         break;   
      case 0x25:  // (or)
         instruct.rd_value = (unsigned int)instruct.rs_value | (unsigned int)instruct.rt_value;
         break;   
      case 0x26:  // (xor)
         instruct.rd_value = (unsigned int)instruct.rt_value ^ (unsigned int)instruct.rt_value;
         break;   
      case 0x00:  // (sll)
         instruct.rd_value = (unsigned int)instruct.rt_value << instruct.shamt;
         break;   
      case 0x02:  // (srl)
         instruct.rd_value = (unsigned int)instruct.rt_value >> instruct.shamt;
         break;   
      case 0x03:  // (sra)
         instruct.rd_value = (int)instruct.rt_value >> instruct.shamt;
         break;   
      case 0x04:  // (sllv)
         instruct.rd_value = (unsigned int)instruct.rt_value << (unsigned int)instruct.rt_value; 
         break;   
      case 0x06:  // (srlv)
         instruct.rd_value = (unsigned int)instruct.rt_value >> (unsigned int)instruct.rt_value; 
         break;   
      case 0x07:  // (srav)
         instruct.rd_value = (int)instruct.rt_value >> (int)instruct.rt_value; 
         break;   
      case 0x2A:  // (slt)
         instruct.rd_value = (int)instruct.rt_value < ((int)instruct.rt_value)? 1:0; 
         break;   
      case 0x2B:  // (sltu)
         instruct.rd_value = (unsigned int)instruct.rt_value < ((unsigned int)instruct.rt_value)? 1:0; 
         break;   
      case 0x08:  // (jr)
      case 0x09:  // (jalr)
          //jump address already calculated
         break;
      }
   return instruct;
}

INST execute_i_helper(INST instruct) {
   printf("opcode = %d\n", instruct.opcode);
   switch (instruct.opcode) {
      case 0x08:   // I Type (addi) 
         instruct.rt_value = (int) instruct.rs_value  + (int) instruct.immed;
         printf("rs (%d) + immed (%d) = rt (%d)\n", instruct.rs_value, instruct.immed, instruct.rt_value);  
         break;    
      case 0x09:  // Type (addiu)
         instruct.rt_value = (unsigned int) instruct.rs_value + (unsigned int) instruct.immed; 
         break;    
      case 0x0C:  // I Type (andi) 
         instruct.rt_value = (unsigned int) instruct.rs_value & (unsigned int) instruct.rt_value; 
         break;    
      case 0x0D:  // I Type (ori)
         instruct.rt_value = (unsigned int) instruct.rs_value | (unsigned int) instruct.immed;
         break;    
      case 0x0E:  // I Type (xori)
         instruct.rt_value = (unsigned int) instruct.rs_value ^ (unsigned int) instruct.immed;
         break;     
      case 0x0A:  // I Type (slti) 
         instruct.rt_value =  (int) instruct.rs_value < (int) instruct.immed;
         break;   
      case 0x0B:  // I Type (sltiu)
         instruct.rt_value =  (unsigned int) instruct.rs_value < (unsigned int) instruct.immed; 
         break;     
   }
   return instruct;
}

INST execute_s_helper(INST instruct) {
   instruct.eff_addr = eff_addr_load(instruct.curr_instruction);
   switch (instruct.opcode) {
      case 0x24:  // I Type (lbu) 
      case 0x25:  // I Type (lhu)
         instruct.heap_addr = (unsigned int)instruct.rt_value + (unsigned int)instruct.eff_addr;
         break; 
      case 0x20:  // I Type (lb)
      case 0x21:  // I Type (lh)
      case 0x0F:  // I Type (lui)
      case 0x23:  // I Type (lw)
         instruct.heap_addr = instruct.rt_value + instruct.eff_addr;
         break;
      case 0x28:  // I Type (sb) 
      case 0x29:  // I Type (sh) 
      case 0x2B:  // I Type (sw) 
         instruct.heap_addr = instruct.rs + instruct.eff_addr;
         break; 
   }
   return instruct;
}

INST execute_b_helper(INST instruct) {
   switch (instruct.opcode) {
      case 0x04:     // I Type (beq) 
         if (instruct.rs == instruct.rt) {
            instruct.brn_addr = instruct.pc + eff_addr(instruct.pc, instruct.immed);
         }
         break;
      case 0x05:     // I Type (bne)
         if (instruct.rs != instruct.rt) {
            instruct.brn_addr = instruct.pc + eff_addr(instruct.pc, instruct.immed);
         }
         break;
   }
   return instruct;
}

INST execute_j_helper(INST instruct) {
    instruct.jmp_addr = jmp_addr(instruct.curr_instruction);
   return instruct;
}

int eff_addr(int pc, int imm_value) {                                           //I Types, Branches only
   return pc + imm_value;
}

unsigned int jmp_addr(MIPS ir) {                                                         //J Types
   unsigned int jmp_addr;

   jmp_addr = (ir & 0x03FFFFFF);

   jmp_addr <<= 2;   
   return jmp_addr;
}

unsigned int eff_addr_load(MIPS ir) {
   unsigned int ext_imm = (ir & 0x0000FFFF);
   ext_imm |= ((ext_imm & 0x8000) ? 0xFFFF0000 : 0);
   return ext_imm;
}
//=============================================================================================================
//=============================================================================================================
