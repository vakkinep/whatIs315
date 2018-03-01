#ifndef READINSTRUCTIONS_H
#define READINSTRUCTIONS_H

//===========STRUCTS
typedef unsigned int MIPS, *MIPS_PTR;
typedef unsigned int REG, *REG_PTR;

typedef struct INSTRUCTION {
	char          type;
	unsigned int  opcode;
	unsigned int  rs;
	unsigned int  rt;
	unsigned int  rd;
	unsigned int  shamt;
	unsigned int  func_code;
	int           immed;
	unsigned int  jmp_addr;
        int  brn_addr;
        unsigned int  eff_addr;
} INST;

#include "startChecks.h"
#include "mips_asm_header.h"

/*----------Prototypes---------*/
int func_code(MIPS ir); 
unsigned int op_code(MIPS ir); 
unsigned int shamt(MIPS ir); 
int imm_val(MIPS ir);
int sign_ext(INST instruction);
int eff_addr(int pc, int imm_value);
char type(unsigned int op_code, MIPS ir);
unsigned int jmp_addr(MIPS ir);
void  printReg(int reg, char* title);
unsigned int eff_addr_ls(MIPS ir);
int reg_s(MIPS ir);
int reg_t(MIPS ir);
int reg_d(MIPS ir);
int readInstructions(int memp, REG reg[], int pc, INST instruct);
int readNextInst(int memp, REG regs[], int pc, INST instruct);
void printValues(INST instruction);
void execute(INST instruction);
//void printValues(INST instruction);

#endif
