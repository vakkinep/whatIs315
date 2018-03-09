/* Test file for execute code */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readInstructions.h"

//================== Global Variables
MIPS mem[1024];	
int haltflag = 0;


INST setup_inst_add(INST instruct) {
	instruct.curr_instruction = 0b000000010000010001000000010100;
	instruct.pc = 0;
	instruct.type = 'r'; 
	instruct.opcode = 0x0;
	instruct.rs = 8;
	instruct.rt = 4;
	instruct.rd = 8;
	instruct.rs_value = 4;
	instruct.rt_value = 6;
	instruct.rd_value = 0;
	instruct.ra = 0;
	instruct.heap_addr = 0;
	instruct.shamt = 0;
	instruct.func_code = 0x20;
	instruct.immed = 0;
	instruct.jmp_addr = 0;
	instruct.brn_addr = 0;
	instruct.eff_addr = 0;
	return instruct;
}

INST setup_inst_addi(INST instruct) {
	instruct.curr_instruction = 0b000000010000010001000000010100;
	instruct.pc = 4;
	instruct.type = 'i'; 
	instruct.opcode = 0x08;
	instruct.rs = 8;
	instruct.rt = 10;
	instruct.rd = 0;
	instruct.rs_value = 4;
	instruct.rt_value = 2;
	instruct.rd_value = 0;
	instruct.ra = 0;
	instruct.heap_addr = 0;
	instruct.shamt = 0;
	instruct.func_code = 0x0;
	instruct.immed = 0x0001;
	instruct.jmp_addr = 0;
	instruct.brn_addr = 0;
	instruct.eff_addr = 0;
	return instruct;
}


int main(int argc, char *argv[]) {
	INST instruct;
	INST result;

	instruct = setup_inst_add(instruct);
	result = execute(instruct);
	printValues(instruct);
	printf("added result: %d\n\n", result.rd_value);

	instruct = setup_inst_addi(instruct);
	printValues(instruct);
	result = execute(instruct);
	printValues(result);
	printf("added immed result: %d\n\n", result.rt_value);
}