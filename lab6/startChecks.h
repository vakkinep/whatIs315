#ifndef STARTCHECKS_H
#define STARTCHECKS_H

#include "mips_asm_header.h"
#include "readInstructions.h"

/*----------Prototypes---------*/
void checkInputs(int argc, char *argv[]);
int checkFile(char *argv[], MIPS* mem, size_t mem_size);

#endif
