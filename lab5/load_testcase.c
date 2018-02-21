/*----------------------------------------------------------------------*
 *	Example mips_asm program loader. This loads the mips_asm binary	*
 *	named "testcase1.mb" into an array in memory. It reads		*
 *	the 64-byte header, then loads the code into the mem array.	*
 *									*
 *	DLR 4/18/16							*
 *----------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mips_asm_header.h"


typedef unsigned int MIPS, *MIPS_PTR;

MB_HDR mb_hdr;		/* Header area */
MIPS mem[1024];		/* Room for 4K bytes */

int func_code(MIPS ir); 
unsigned int op_code(MIPS ir); 
void shamt(MIPS ir); 
int imm_val(MIPS ir);
void eff_addr(int pc, int imm_value);
char type(unsigned int op_code, MIPS ir);
void jmp_addr(MIPS ir);


int main(int argc, char *argv[]) {
	FILE *fd;
	int n;
	int memp;
	int i;
	int curr_instruction;
	int imm_value;
    char *filename;	/* This is the filename to be loaded */

    if (argc <= 1) {
    	printf("Missing File name - quitting.\n");
    	exit(1);
    }

    filename = argv[1];

   /* format the MIPS Binary header */

	fd = fopen(filename, "rb");
	if (fd == NULL) { printf("\nCouldn't load test case - quitting.\n"); exit(99); }

   memp = 0;		/* This is the memory pointer, a byte offset */

   /* read the header and verify it. */
	fread((void *) &mb_hdr, sizeof(mb_hdr), 1, fd);
	if (! strcmp(mb_hdr.signature, "~MB")==0)
		{ printf("\nThis isn't really a mips_asm binary file - quitting.\n"); exit(98); }

	printf("\nInput file: %s \n\n", filename, mb_hdr.size);

   /* read the binary code a word at a time */

	do {
      n = fread((void *) &mem[memp/4], 4, 1, fd); /* note div/4 to make word index */
		if (n) 
         memp += 4;	/* Increment byte pointer by size of instr */
			else
				break;       
		} while (memp < sizeof(mem)) ;

		fclose(fd);


   for (i = 0; i<memp; i+=4) {	/* i contains byte offset addresses */
		unsigned int opcode;
		char instruct_type;
		curr_instruction = mem[i/4];
		printf("@PC=%08X, ", i);
         
		opcode = op_code(curr_instruction);		//Print Opcode
		instruct_type = type(opcode, curr_instruction);			//Print instruction type

		//if R-Type
        if (instruct_type == 'r') { 
	        if (func_code(curr_instruction)) {			//print function code
	       		shamt(curr_instruction); 			//print shift amount
	    	}
	    }
	   
   		//if I-Type, print immediate value
		if (instruct_type == 'i'|| instruct_type == 'b' || instruct_type == 's') {
			imm_value = imm_val(curr_instruction);
		}
   		
   		//if I-Type (Branch), print branch address
		if (instruct_type == 'b') {                     
			eff_addr(i, imm_value);
		}

		//if J-Type, print Jump Address
		if (instruct_type == 'j') {
			jmp_addr(curr_instruction);
		}     

		printf("\n\n");
	}

	printf("\n");

	exit(0);
}

/************************************************************************************************
  Helper functions

 ************************************************************************************************/

unsigned int op_code(MIPS ir) {
	unsigned int op_code;

    op_code = ((ir >> 26) & 0x00000003F);    //Mask all but opcode
    //printf("OpCode=0x%02X, ", op_code);
    return op_code;
}

int imm_val(MIPS ir) {              //I Types
	unsigned int imm_val;
	unsigned int sign_ext;
   	int sign;

	imm_val = (ir & 0x0000FFFF); 
	printf("Imm=0x%04X, ", imm_val);

	sign = (imm_val & 0x00008000);
   	if (sign) {
    	sign_ext = (imm_val | 0xFFFF0000);
   	}
   	printf("signext: 0x%08X (%d)", sign_ext, (int) sign_ext);
	return imm_val;
}

void shamt(MIPS ir) {               //R Types
	unsigned int shamt;

	shamt = ((ir >> 6) & 0x00000001F);    
	printf("shamt=0x%02X ", shamt);
}

int func_code(MIPS ir) {           //R Types
	unsigned int func_code;

	func_code = (ir & 0x0000003F);
	
	switch (func_code) {
		case 0x20: printf("R Type, Function=0x%02X (add)\n", func_code); break;
		case 0x21: printf("R Type, Function=0x%02X (addu)\n", func_code); break;
		case 0x22: printf("R Type, Function=0x%02X (sub)\n", func_code); break;
		case 0x23: printf("R Type, Function=0x%02X (subu)\n", func_code); break;
		case 0x24: printf("R Type, Function=0x%02X (and)\n", func_code); break;
		case 0x27: printf("R Type, Function=0x%02X (nor)\n", func_code); break;
		case 0x25: printf("R Type, Function=0x%02X (or)\n", func_code); break;
		case 0x26: printf("R Type, Function=0x%02X (xor)\n", func_code); break;
		case 0x00: printf("R Type, Function=0x%02X (sll)\n", func_code); break;
		case 0x02: printf("R Type, Function=0x%02X (srl)\n", func_code); break;
		case 0x03: printf("R Type, Function=0x%02X (sra)\n", func_code); break;
		case 0x04: printf("R Type, Function=0x%02X (sllv)\n", func_code); break;
		case 0x06: printf("R Type, Function=0x%02X (srlv)\n", func_code); break;
		case 0x07: printf("R Type, Function=0x%02X (srav)\n", func_code); break;
		case 0x2A: printf("R Type, Function=0x%02X (slt)\n", func_code); break;
		case 0x2B: printf("R Type, Function=0x%02X (sltu)\n", func_code); break;
		case 0x08: printf("R Type, Function=0x%02X (jr)\n", func_code); break;
		case 0x09: printf("R Type, Function=0x%02X (jalr)\n", func_code); break;
		case 0x1A: printf("R Type, Function=0x%02X (div)\n", func_code); break;
		case 0x1B: printf("R Type, Function=0x%02X (divu)\n", func_code); break;
		case 0x18: printf("R Type, Function=0x%02X (mult)\n", func_code); break;
		case 0x19: printf("R Type, Function=0x%02X (multu)\n", func_code); break;
		default:
			printf("0x%08X - Invalid Instruction.\n", ir);
			return 0;
	}
	return 1;
}

void eff_addr(int pc, int imm_value) {  //I Types, Branches only
	int eff_addr;

	eff_addr = pc + imm_value;
	printf(", BranchAddr=0x%08X ", eff_addr);
}

void jmp_addr(MIPS ir) {  //J Types
	unsigned int jmp_addr;

	jmp_addr = (ir & 0x03FFFFFF);
	printf("addr=0x%06X \n", jmp_addr);

	jmp_addr <<= 2;	
	printf("JumpAddr=0x%08X ", jmp_addr);
}

// Helpers:
//
// Find Type:
// r: R Type, j: J-Type, i: I Type, b: I Type (branch), s: I Type (load/store)
char type(unsigned int op_code, MIPS ir) {
	switch(op_code) {
		case 0x00: return 'r';
		case 0x02: printf("OpCode=0x%02X, J Type (j), ", op_code); return 'j';
		case 0x03: printf("OpCode=0x%02X, J Type (jal), ", op_code); return 'j';
		case 0x08: printf("OpCode=0x%02X, I Type (addi)\n", op_code); return 'i';
		case 0x09: printf("OpCode=0x%02X, I Type (addiu)\n", op_code); return 'i';
		case 0x0C: printf("OpCode=0x%02X, I Type (andi)\n", op_code); return 'i';
		case 0x0D: printf("OpCode=0x%02X, I Type (ori)\n", op_code); return 'i';
		case 0x0E: printf("OpCode=0x%02X, I Type (xori)\n", op_code); return 'i';
		case 0x0A: printf("OpCode=0x%02X, I Type (slti)\n", op_code); return 'i';
		case 0x0B: printf("OpCode=0x%02X, I Type (sltiu)\n", op_code); return 'i';
		case 0x04: printf("OpCode=0x%02X, I Type (beq)\n", op_code); return 'b';
		case 0x05: printf("OpCode=0x%02X, I Type (bne)\n", op_code); return 'b';
		case 0x20: printf("OpCode=0x%02X, I Type (lb)\n", op_code); return 's';
		case 0x24: printf("OpCode=0x%02X, I Type (lbu)\n", op_code); return 's';
		case 0x21: printf("OpCode=0x%02X, I Type (lh)\n", op_code); return 's';
		case 0x25: printf("OpCode=0x%02X, I Type (lhu)\n", op_code); return 's';
		case 0x0F: printf("OpCode=0x%02X, I Type (lui)\n", op_code); return 's';
		case 0x23: printf("OpCode=0x%02X, I Type (lw)\n", op_code); return 's';
		case 0x28: printf("OpCode=0x%02X, I Type (sb)\n", op_code); return 's';
		case 0x29: printf("OpCode=0x%02X, I Type (sh)\n", op_code); return 's';
		case 0x2B: printf("OpCode=0x%02X, I Type (sw)\n", op_code); return 's';
		case 0x31: printf("OpCode=0x%02X, I Type (lwcl)\n", op_code); return 's';
		case 0x35: printf("OpCode=0x%02X, I Type (ldcl)\n", op_code); return 's';
		case 0x39: printf("OpCode=0x%02X, I Type (swcl)\n", op_code); return 's';
		case 0x3D: printf("OpCode=0x%02X, I Type (sdcl)\n", op_code); return 's';
		default:
			printf("0x%08X - Invalid Instruction.\n", ir);
			return '0';
	}
}