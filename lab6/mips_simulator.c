#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readInstructions.h"

//=================== GLOBAL VARIABLES
MIPS mem[1024];	
//=================== Prototypes
void printRegisters(REG* regs);
int runSimulator(char mode, int memp, REG regs[], int pc, int* inst_ran, int* clocks, int* mem_ref);




int runSimulator(char mode, int memp, REG regs[], int pc, int* inst_ran, int* clocks, int* mem_ref) {
   INST instruct;
   if (mode == 's') {
      //run one instruction and print
      if (pc >= memp) {
         printf("Done\n\n");
         printf("Instructions ran %d\nClock cycles used: %d\nTimes referenced memory %d\n\n", *inst_ran, *clocks, *mem_ref);
         exit(0);
      }
      else {
         pc = readNextInst(memp, regs, pc, instruct, inst_ran, clocks, mem_ref);
         printRegisters(regs); 
      }
   } else if (mode == 'r') {
      //run all instructions and print only end result
      pc = readInstructions(memp, regs, pc, instruct, inst_ran, clocks, mem_ref);
      printRegisters(regs);     //prints final register values not values? 
      printf("\n\n");
      printf("Instructions ran %d\nClock cycles used: %d\nTimes referenced memory %d\n\n", *inst_ran, *clocks, *mem_ref);
      exit(0);
   }
   else {
      printf("ERROR - USAGE \n\t 'r' - run \n\t 's' - single step\n\t 'x' - exit\n\n");
   }
   return pc;
}

void printRegisters(REG* regs) {
   int i = 0;
   for (i = 0 ; i < 32 ; i++) {
      if ((i % 3) == 0 && i > 0) {
         printf("\n");
      }
      printf("REG[%2d]:\t 0x%08X\t\t", i, regs[i]);
   }
}

/*==================================TO DO======================================
 *     1.Print statistics
 *        a. Clock cycles 
 *        b. Number of memory references (read/writes)
 *        c. Number of instructions executed
 *        d. Print contents of the registers
 *     2.Simulate memory
 *        a. Init 32 registers to 0 (unigned int array reg[32])
 *        b. Create "base" of 0x400000
 *        c. Create an offset for fed in pointers
 *     3.Print results of the run
 *     4.End if syscall with function code of 10 in $v0
 *     5.Take input for single step and wait for either "run" to change mode or 
 *        input for next step.
 *     6.
 *
 *
 *
 *=============================================================================*/


/* Functions: 
 *     PrintRegisters(); //Prints out the registers and their contents, useful for single step and end of run
 *     AccessMemory();   //Uses the memory offset to access the memory heap and write or read
 *     
 */

int main(int argc, char *argv[]) {
   char ch;
   int memp;                                                                     //Size of pulled instructions
   int pc = 0;
   REG regs[32] = {0};
   int inst_ran = 0;
   int clocks = 0;
   int mem_ref = 0;

   checkInputs(argc, argv);
   memp = checkFile(argv, mem, sizeof(mem));

   while ( 1 ) {
      char t;
      printf("\nChoose Mode ('r' - run, 's' - single step, 'x' - exit): ");
      ch=getchar();

      if (ch == 'x') {
         printf("Exiting.\n");
         printf("Instructions ran %d\nClock cycles used: %d\nTimes referenced memory %d\n\n", inst_ran, clocks, mem_ref);
         break;
      }
      pc = runSimulator(ch, memp, regs, pc, &inst_ran, &clocks, &mem_ref);
      while ((t = getchar()) != EOF && t != '\n'); 								//loop through rest of stdin
   }
}
