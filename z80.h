#ifndef __z80__ 
#define __z80__
#include "stdint.h"


/* constants */
#define MEMORYSIZE 65536
#define CARRY 4
#define HALFCARRY 5
#define SUB 6
#define ZERO 7


/*Data types*/

typedef struct _HlByte { /*TODO check endianness */
	uint8_t h;
	uint8_t l;
} HlByte;

typedef struct _z80 {
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	union{
		uint16_t hl;
		HlByte hlByte;
	};
	uint16_t pc;
	uint16_t sp;
	uint8_t f;
} z80;




/* functions defined as macros */

#define setFlag(X,FLAG) (X | (1<<(FLAG)))
#define getFlag(X,FLAG) (X & (1<<(FLAG)))

#define nibbleSwap(X) ((X >> 4 & 0b00001111) + (X << 4 & 0b11110000))


/* general purpose functions */
	
	uint8_t buildStatusFlag(int zero, int sub, int halfcarry,int carry);

/* opcode functions here */


#endif
