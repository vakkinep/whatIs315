#ifndef READINSTRUCTIONS_H
#define READINSTRUCTIONS_H

//===========STRUCTS
typedef unsigned int MIPS, *MIPS_PTR;

#include "startChecks.h"
#include "mips_asm_header.h"

/*----------Prototypes---------*/
int func_code(MIPS ir); 
unsigned int op_code(MIPS ir); 
void shamt(MIPS ir); 
int imm_val(MIPS ir);
void eff_addr(int pc, int imm_value);
char type(unsigned int op_code, MIPS ir);
void jmp_addr(MIPS ir);
void printReg(int reg, char* title);
int res_s(MIPS ir);
int res_t(MIPS ir);
int res_d(MIPS ir);
void eff_addr_ls(MIPS ir);
void printRegRS(MIPS ir);
void readInstructions(int memp);

#endif
