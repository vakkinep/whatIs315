#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pipelined.h"
#include "startChecks.h"

//=================== GLOBAL VARIABLES
MIPS mem[1024];	
//=================== Prototypes
int runSimulator(int memp, REG regs[], int pc, int* inst_ran, int* clocks, int* mem_ref);

int runSimulator(int memp, REG regs[], int pc, int* inst_ran, int* clocks, int* mem_ref) {
   INST instruct_1;
   INST instruct_2;
   INST instruct_3;
   INST instruct_4;
   INST instruct_5;

      //run all instructions and print only end result
      for () {
         writeback(instruct_5);             
         instruct_5 = mem(instruct_4);             
         instruct_4 = execute(instruct_3);             
         //IF FLUSH FIND OUT THEN SET 3,2,1 ALL TO NOPS
         instruct_3 = decode(instruct_2);             
         instruct_2 = fetch(instruct_1);             
      }
      printf("\n\n");
      printf("Instructions ran %d\nClock cycles used: %d\nTimes referenced memory %d\n\n", *inst_ran, *clocks, *mem_ref);
      exit(0);
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
