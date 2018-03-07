/*----------------------------------------------------------------------
 * Lab 7 (MIPS Pipeline Simulator) - CPE 315
 * Jenna Stephens
 * Venkat Akkinepally
 * Austin Whaley
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "startChecks.h"

void checkInputs(int argc, char *argv[]) {
   if (argc <= 1) {
      printf("Missing File name - quitting.\n");
      exit(1);
   }
   else if (argc > 2) {
      printf("Too many arguments - quitting.\n");
      exit(1);
   }
}

int checkFile(char *argv[], MIPS* mem, size_t mem_size) {
   FILE* fd;
   int memp;
   char *filename;	                                                         /* This is the filename to be loaded */
   int n;
   MB_HDR mb_hdr;

   filename = argv[1];
   /* format the MIPS Binary header */
   fd = fopen(filename, "rb");
   if (fd == NULL) { 
      printf("\nCouldn't load test case - quitting.\n"); 
      exit(99); 
   }

   memp = 0;		                                                         /* This is the memory pointer, a byte offset */

   /* read the header and verify it. */
   fread((void *) &mb_hdr, sizeof(mb_hdr), 1, fd);
   if (! strcmp(mb_hdr.signature, "~MB")==0)   { 
      printf("\nThis isn't really a mips_asm binary file - quitting.\n"); 
      exit(98); 
   }
   
   printf("\nInput file: %s \n\n", filename, mb_hdr.size);

   /* read the binary code a word at a time */
   do {
      n = fread((void *) &mem[memp/4], 4, 1, fd);                                /* note div/4 to make word index */
      if (n) 
         memp += 4;	                                                         /* Increment byte pointer by size of instr */
      else
         break;       
   } while (memp < mem_size) ;

   fclose(fd);
   return memp;
}
