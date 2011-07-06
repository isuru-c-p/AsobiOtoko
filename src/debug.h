#include "stdint.h"
#include "stdio.h"
#include "z80.h"
const char * dissasemble(uint8_t b,int secondary);

#define printCPU(cpu) fprintCPU(stdout,cpu)

void
fprintCPU(FILE*out,z80*pz80);
