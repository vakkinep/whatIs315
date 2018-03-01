#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readInstructions.h"

int runSimulator(char mode) {
   if (mode == 's') {
      //run one instruction and print
   } else if (mode == 'r') {
      //run all instructions and print only end result
   }
   else {
      printf("ERROR - USAGE \n\t 'r' - run \n\t 's' - single step\n\n");
   }
   return 0;
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
*     
*     
*     
*     
*/
int main(int argc, char *argv[]) {
	char ch;
	//printf("\nEnter mode ('r' - run, 's' - single step): \n");
	while ( 1 ) {
		char t;
		printf("\nChoose Mode ('r' - run, 's' - single step, 'x' - exit): ");
		ch=getchar();
	
		if (ch == 'x') {
			printf("Exiting.\n");
			break;
		}
		runSimulator(ch);
		while ((t = getchar()) != EOF && t != '\n'); 								//loop through rest of stdin
	}
}