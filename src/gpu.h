#ifndef __gpu__
#define __gpu__

#include <stdint.h>

#define VRAM_SIZE 0

#define BGWON 0
#define OBJON 1
#define OBJSIZE 2
#define BGMAP 3
#define BGWDATASEL 4
#define WDISP 5
#define WMAP 6
#define LCDON 7

// macros

#define getLCDCBit(pgpu, X) (((pgpu)->LCDC >> (X)) & 0x01)

#define getPixel(pgpu, addr, x) (((pgpu)->vram[(addr)] >> (x)) & 0x01)

// data types

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
	uint8_t OBP0;
	uint8_t OBP1;
	uint8_t WY;
	uint8_t WX;
	int clock;
} GPU;

uint8_t gpu_rb(GPU*pgpu, uint16_t addr);
void gpu_wb(GPU*pgpu, uint16_t addr, uint8_t val);
uint8_t getPixelColor(GPU*pgpu, uint8_t pixel);

void initGPU(GPU*pgpu);
void gpu_step(GPU*pgpu, int tcycles);
void writeScanline(GPU*pgpu);
void renderScreen(GPU*pgpu);

#endif
