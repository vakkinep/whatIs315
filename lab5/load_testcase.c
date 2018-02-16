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

void op_code(MIPS ir); 

main()
{
   FILE *fd;
   int n;
   int memp;
   int i;
   int curr_instruction;
   char filename[] = "testcase1.mb";	/* This is the filename to be loaded */

   /* format the MIPS Binary header */

   fd = fopen(filename, "rb");
   if (fd == NULL) { printf("\nCouldn't load test case - quitting.\n"); exit(99); }

   memp = 0;		/* This is the memory pointer, a byte offset */

   /* read the header and verify it. */
   fread((void *) &mb_hdr, sizeof(mb_hdr), 1, fd);
   if (! strcmp(mb_hdr.signature, "~MB")==0)
   { printf("\nThis isn't really a mips_asm binary file - quitting.\n"); exit(98); }

   printf("\n%s Loaded ok, program size=%d bytes.\n\n", filename, mb_hdr.size);

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

   for (i = 0; i<memp; i+=4)	/* i contains byte offset addresses */
   {
      curr_instruction = mem[i/4];
      printf("Instruction@%08X : %08X\n", i, curr_instruction);
      //=============================================================================
      //                         Add code here
      //
      //             -Print out type of instruction   (R/I/J?) 
      //             -Print out Opcode                (HEX?)
      op_code(curr_instruction);
      //             -Print out Function Code         (HEX?)
      //             -Print out registers             (Source/Target/Dest)            
      //             -Print out immedval              (Sign extended or no)
      //             -Print out shamt                 (Rshifted and amt)
      //             -Print out effective address
      //             -Print out branch compares
      //             -Print out bad instruction
      //=============================================================================
   }
   printf("\n");

   exit(0);
}

void op_code(MIPS ir) {
   unsigned int op_code;
   
   op_code = ((ir >> 26) & 0x00000003F);    //Mask all but opcode

   printf("OpCode = 0x%02X\n", op_code);

}
// Helpers:
//
// Find type - switch or waterfall if's
// When found print relevant stuff
// 
