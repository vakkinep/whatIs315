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

unsigned int jmp_addr(MIPS ir) {                                                         //J Types
   unsigned int jmp_addr;

   jmp_addr = (ir & 0x03FFFFFF);

   jmp_addr <<= 2;   
   return jmp_addr;
}

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
         //jump address already calculated, nothing to do
         break;
      case 0x09:  // (jalr)
         instruct.ra = instruct.pc + 4;               //save return address
         //jump address already calculated
         break;
      }
   return instruct;
}

INST execute_i_helper(INST instruct) {
   switch (instruct.opcode) {
      case 0x08:   // I Type (addi) 
         instruct.rt_value = (int) instruct.rt_value  + (int) instruct.immed;  
         break;    
      case 0x09:  // Type (addiu)
         instruct.rt_value = (unsigned int) instruct.rt_value + (unsigned int) instruct.immed; 
         break;    
      case 0x0C:  // I Type (andi) 
         instruct.rt_value = (unsigned int) instruct.rt_value & (unsigned int) instruct.immed; 
         break;    
      case 0x0D:  // I Type (ori)
         instruct.rt_value = (unsigned int) instruct.rt_value | (unsigned int) instruct.immed;
         break;    
      case 0x0E:  // I Type (xori)
         instruct.rt_value = (unsigned int) instruct.rt_value ^ (unsigned int) instruct.immed;
         break;     
      case 0x0A:  // I Type (slti) 
         instruct.rt_value =  (int) instruct.rt_value < (int) instruct.immed;
         break;   
      case 0x0B:  // I Type (sltiu)
         instruct.rt_value =  (unsigned int) instruct.rt_value < (unsigned int) instruct.immed; 
         break;     
   }
   return instruct;
}

INST execute_s_helper(INST instruct) {
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
         instruct_heap_addr = instruct.rs + instruct.eff_addr;
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
   switch (instruct.opcode) {
      case 0x02:  // (j)
      //Jump address already calculated, nothing to do
      break;
      case 0x03:  // (jal)
      //jump address already calculated
      break;
   }
   return instruct;
}


INST execute(INST instruct) {
   if (instruct.type == 'r') {      //R Types
      return execute_r_helper(instruct);
   }

   else if (instruct.type == 'i') {    //I Types
      return execute_i_helper(instruct);
   }

   else if (instruct.type == 's') {    //Load and Store Types
      return execute_s_helper(instruct);
   }

   else if (instruct.type == 'b') {    //Branch Type
      return execute_b_helper(instruct);
   }

   else if (instruct.type == 'j') {    //Jump Type
      return execute_j_helper(instruct);
   }   
   return instruct;
}


INST mem(INST instruct) {
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

   } else if (instruct.type == 'b') {  //Branch
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

   } else if (instruct.type == 'j') {     //Jump
      switch (instruct.opcode) {
         if (instruct.opcode == 0x03) {  // (jal)
            instruct.ra = instruct.pc + 4;
         }
         instruct.pc = instruct.jmp_addr;
   }
}
