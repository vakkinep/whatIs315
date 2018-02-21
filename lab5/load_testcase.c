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

void func_code(MIPS ir); 
unsigned int op_code(MIPS ir); 
void shamt(MIPS ir); 
int imm_val(MIPS ir);
void eff_addr(int pc, int imm_value);
char type(unsigned int op_code);


int main(int argc, char **argv) {
	FILE *fd;
	int n;
	int memp;
	int i;
	int curr_instruction;
	int imm_value;
   char filename[] = "testcase1.mb";	/* This is the filename to be loaded */

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


   /* ok, now dump out the instructions loaded: */

   for (i = 0; i<memp; i+=4) {	/* i contains byte offset addresses */
		unsigned int opcode;
		char instruct_type;
		curr_instruction = mem[i/4];
		printf("@PC=%08X, ", i);
    //=============================================================================
    //                         Add code here
    //

	//================================================================================      
    //              -Print out Opcode                 (HEX?)                     AUSTIN
		opcode = op_code(curr_instruction);
	//			  -Print out type of instruction      (R/I/J?)                   JENNA 
		instruct_type = type(opcode);
	//             -Print out Function Code           (HEX?)                     AUSTIN
        if (instruct_type == 'r') { 
	       func_code(curr_instruction); 
        }
	//=================================================================================      

    //             -Print out registers                (Source/Target/Dest)     VINNY            

    //=================================================================================   
    //             -Print out immedval                 (Sign extended or no)    AUSTIN
		if (instruct_type == 'i'|| instruct_type == 'b' || instruct_type == 's') {
			imm_value = imm_val(curr_instruction);
		}
    //             -Print out shamt                    (Rshifted and amt)       AUSTIN
		if (instruct_type == 'r') {
           shamt(curr_instruction);
        }
    //             -Print out effective address                                 AUSTIN
		if (instruct_type == 'b') {                     
			eff_addr(i, imm_value);
		}
//=====================================================================================      

		printf("\n\n");
	}

	printf("\n");

	exit(0);
}

unsigned int op_code(MIPS ir) {
	unsigned int op_code;

    op_code = ((ir >> 26) & 0x00000003F);    //Mask all but opcode
    printf("OpCode=0x%02X, ", op_code);
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

void func_code(MIPS ir) {           //R Types
	unsigned int func_code;

	func_code = (ir & 0x0000003F);
	printf("Function=0x%02X ", func_code);
	switch (func_code) {
		case 0x20: printf("(add)\n"); break;
		case 0x21: printf("(addu)\n"); break;
		case 0x22: printf("(sub)\n"); break;
		case 0x23: printf("(subu)\n"); break;
		case 0x24: printf("(and)\n"); break;
		case 0x27: printf("(nor)\n"); break;
		case 0x25: printf("(or)\n"); break;
		case 0x26: printf("(xor)\n"); break;
		case 0x00: printf("(sll)\n"); break;
		case 0x02: printf("(srl)\n"); break;
		case 0x03: printf("(sra)\n"); break;
		case 0x04: printf("(sllv)\n"); break;
		case 0x06: printf("(srlv)\n"); break;
		case 0x07: printf("(srav)\n"); break;
		case 0x2A: printf("(slt)\n"); break;
		case 0x2B: printf("(sltu)\n"); break;
		case 0x08: printf("(jr)\n"); break;
		case 0x09: printf("(jalr)\n"); break;
		case 0x1A: printf("(div)\n"); break;
		case 0x1B: printf("(divu)\n"); break;
		case 0x18: printf("(mult)\n"); break;
		case 0x19: printf("(multu)\n"); break;
		default:
			printf("Invalid Instruction\n");
	}
}

void eff_addr(int pc, int imm_value) {  //I Types, Branches only
	int eff_addr;

	eff_addr = pc + imm_value;
	printf(", BranchAddr=0x%08X ", eff_addr);
}

void jmp_addr(int imm_value) {  //J Types -- Is this right?
	int jmp_addr = imm_value >> 2;
	printf("JumpAddr=0x%08X ", jmp_addr);
}

// Helpers:
//
// Find Type:
// r: R Type, j: J-Type, i: I Type, b: I Type (branch), s: I Type (load/store)
char type(unsigned int op_code) {
	switch(op_code) {
		case 0x00: printf("R type, "); return 'r';
		case 0x02: printf("J Type (j)\n"); return 'j';
		case 0x03: printf("J Type (jal)\n"); return 'j';
		case 0x08: printf("I Type (addi)\n"); return 'i';
		case 0x09: printf("I Type (addiu)\n"); return 'i';
		case 0x0C: printf("I Type (andi)\n"); return 'i';
		case 0x0D: printf("I Type (ori)\n"); return 'i';
		case 0x0E: printf("I Type (xori)\n"); return 'i';
		case 0x0A: printf("I Type (slti)\n"); return 'i';
		case 0x0B: printf("I Type (sltiu)\n"); return 'i';
		case 0x04: printf("I Type (beq)\n"); return 'b';
		case 0x05: printf("I Type (bne)\n"); return 'b';
		case 0x20: printf("I Type (lb)\n"); return 's';
		case 0x24: printf("I Type (lbu)\n"); return 's';
		case 0x21: printf("I Type (lh)\n"); return 's';
		case 0x25: printf("I Type (lhu)\n"); return 's';
		case 0x0F: printf("I Type (lui)\n"); return 's';
		case 0x23: printf("I Type (lw)\n"); return 's';
		case 0x28: printf("I Type (sb)\n"); return 's';
		case 0x29: printf("I Type (sh)\n"); return 's';
		case 0x2B: printf("I Type (sw)\n"); return 's';
		case 0x31: printf("I Type (lwcl)\n"); return 's';
		case 0x35: printf("I Type (ldcl)\n"); return 's';
		case 0x39: printf("I Type (swcl)\n"); return 's';
		case 0x3D: printf("I Type (sdcl)\n"); return 's';
		default:
		printf("Invalid Instruction\n");
		return '0';
	}
}