#ifndef STARTCHECKS_H
#define STARTCHECKS_H

#include "mips_asm_header.h"
#include "readInstructions_AUSTIN.h"

/*----------Prototypes---------*/
void checkInputs(int argc, char *argv[]);
int checkFiles(char *argv[], MIPS* mem, size_t mem_size);

#endif