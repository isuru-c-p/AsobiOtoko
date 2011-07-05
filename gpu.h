#ifndef __gpu__
#define __gpu__

#include <stdint.h>

#define VRAM_SIZE 0

typedef struct _gpu {
	int mode;
	uint8_t vram[VRAM_SIZE];
	uint8_t SCX; //scrollX
	uint8_t SCY; //scrollY
	uint8_t LY; // current vertical line
	uint8_t LYC; 
	uint8_t STAT;
	uint8_t LCDC;
	uint8_t BGP; // BG palette data
	int clock;
} GPU;

uint8_t rb(GPU*pgpu, uint16_t addr);
void wb(GPU*pgpu, uint16_t addr, uint8_t val);

void step(GPU*pgpu, int tcycles);

#endif
