#ifndef READINSTRUCTIONS_H
#define READINSTRUCTIONS_H

//===========STRUCTS
typedef unsigned int MIPS, *MIPS_PTR;

#include "startChecks.h"
#include "mips_asm_header.h"

/*----------Prototypes---------*/
int func_code(MIPS ir); 
unsigned int op_code(MIPS ir); 
unsigned int shamt(MIPS ir); 
int imm_val(MIPS ir);
int eff_addr(int pc, int imm_value);
char type(unsigned int op_code, MIPS ir);
unsigned int jmp_addr(MIPS ir);
void  printReg(int reg, char* title);
unsigned int eff_addr_ls(MIPS ir);
int reg_s(MIPS ir);
int reg_t(MIPS ir);
int reg_d(MIPS ir);
void readInstructions(int memp);
void printRegRS(MIPS ir);
//void printValues(INST instruction);

#endif
