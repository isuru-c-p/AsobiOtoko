#ifndef __z80__ 
#define __z80__
#include "stdint.h"

#define MEMORYSIZE 65536


#define CARRY 4
#define HALFCARRY 5
#define SUB 6
#define ZERP 7

#define setFlag(X,FLAG) (X |= (1<<(FLAG)))
#define getFlag(X,FLAG) (X & (1<<(FLAG)))



typedef z80 struct _z80 {

	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	union{
		uint16_t hl;
		struct {
			uint8_t h;
			uint8_t l;
		};
	}
	uint16_t sp;
	uint8_t f;
	uint8_t memory[MEMORYSIZE];
};

#endif
