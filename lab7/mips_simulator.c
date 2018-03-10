#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pipelined.h"
#include "startChecks.h"

//=================== GLOBAL VARIABLES
MIPS mem[1024];
unsigned int heap[4096];	
int haltflag;
//=================== Prototypes
int runSimulator(int memp, REG regs[], int pc, int* inst_ran, int* clocks, int* mem_ref);

int runSimulator(int memp, REG regs[], int pc, int* inst_ran, int* clocks, int* mem_ref) {
   INST instruct_1;
   INST instruct_2;
   INST instruct_3;
   INST instruct_4;
   INST instruct_5;
   haltflag = 0;

   initialize_inst(&instruct_1);
   initialize_inst(&instruct_2);
   initialize_inst(&instruct_3);
   initialize_inst(&instruct_4);
   initialize_inst(&instruct_5);
   printf("entering loop\n");
      //run all instructions and print only end result
      while (!haltflag) {
         writeback(instruct_5, regs);             
         instruct_5 = memory(instruct_4);             
         instruct_4 = execute(instruct_3);             
         //IF FLUSH FIND OUT THEN SET 3,2,1 ALL TO NOPS
         instruct_3 = decode(instruct_2, regs);             
         instruct_2 = fetch(instruct_1, mem);
         instruct_1.pc += 1;         
         (*clocks)++;
      }
      printf("\n\n");
      printf("Instructions ran %d\nClock cycles used: %d\nTimes referenced memory %d\n\n", *inst_ran, *clocks, *mem_ref);
}

void initialize_inst(INST_PTR instruct) {
   instruct->curr_instruction = 0;
   instruct->pc = 0;
   instruct->type = 'n'; 
   instruct->opcode = 0x0;
   instruct->rs = 0;
   instruct->rt = 0;
   instruct->rd = 0;
   instruct->rs_value = 0;
   instruct->rt_value = 0;
   instruct->rd_value = 0;
   instruct->ra = 0;
   instruct->heap_addr = 0;
   instruct->shamt = 0;
   instruct->func_code = 0x0;
   instruct->immed = 0;
   instruct->jmp_addr = 0;
   instruct->brn_addr = 0;
   instruct->eff_addr = 0;
}

int main(int argc, char *argv[]) {
   int memp;                                                                     //Size of pulled instructions
   int pc = 0;
   REG regs[32] = {0};
   int inst_ran = 0;
   int clocks = 0;
   int mem_ref = 0;

   checkInputs(argc, argv);
   memp = checkFile(argv, mem, sizeof(mem));
   pc = runSimulator(memp, regs, pc, &inst_ran, &clocks, &mem_ref);
   return EXIT_SUCCESS;
}
