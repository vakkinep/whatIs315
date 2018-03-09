#ifndef PIPELINED_H
#define PIPELINED_H
//===========STRUCTS
typedef unsigned int MIPS, *MIPS_PTR;
typedef unsigned int REG, *REG_PTR;

typedef struct INSTRUCTION {
    unsigned int  pc;
    MIPS          curr_instruction;
    char          type;
    unsigned int  opcode;
    unsigned int  rs;
    unsigned int  rt;
    unsigned int  rd;
    unsigned int  rs_value;
    unsigned int  rt_value;
    unsigned int  rd_value;
    unsigned int  ra;
    unsigned int  heap_addr;
    unsigned int  shamt;
    unsigned int  func_code;
    int           immed;
    int           sign_ext;
    unsigned int  jmp_addr;
    int           brn_addr;
    unsigned int  eff_addr;
} INST, *INST_PTR;

/*----------Prototypes---------*/
int func_code(MIPS ir); 
unsigned int op_code(MIPS ir); 
unsigned int shamt(MIPS ir); 
int imm_val(MIPS ir);
int sign_extend(INST instruction);
char type(unsigned int op_code, MIPS ir);
int reg_s(MIPS ir);
int reg_t(MIPS ir);
int reg_d(MIPS ir);

#endif
