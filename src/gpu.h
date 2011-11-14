#ifndef __gpu__
#define __gpu__

#include <stdint.h>

#define VRAM_SIZE 8192
#define OAM_SIZE 160

enum{
BGWON = 0,
OBJON,
OBJSIZE, 
BGMAP ,
BGWDATASEL, 
WDISP ,
WMAP ,
LCDON 
};

#define SCREEN_BUFF_SIZE 160*144

#define PRIORITY 7
#define YFLIP 6
#define XFLIP 5
#define SPRITE_PALETTE 4

// macros

#define getLCDCBit(pgpu, X) (((pgpu)->LCDC >> (X)) & 0x01)

#define getPixel(pgpu, addr, x) (((pgpu)->vram[(addr)] >> (7-(x))) & 0x01)



#define getSpriteAttr(pgpu, sprite, attr) (((pgpu)->oam[(sprite*4) + 3] >> (attr)) & 0x1)
#define getSpriteX(pgpu, sprite) ((pgpu)->oam[(sprite)*4])
#define getSpriteY(pgpu, sprite) ((pgpu)->oam[((sprite)*4) + 1])
#define getSpriteTile(pgpu, sprite) ((pgpu)->oam[((sprite)*4) + 2])

#define LYCLY_INT 6
#define MODE_2_INT 5
#define MODE_1_INT 4
#define MODE_0_INT 3

#define getStatInterruptEnable(pgpu, interrupt) (((pgpu)->STAT >> (interrupt)) & 0x01)

// data types

typedef struct _sprite {
	uint8_t x;
	uint8_t y;
	uint8_t pattern;
	uint8_t flags;
} Sprite;

typedef struct _gpu {
	int mode;
	uint8_t vram[VRAM_SIZE];
	uint8_t oam[OAM_SIZE];
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
	uint8_t WY_current;
	uint8_t WX;
	uint8_t buffer[SCREEN_BUFF_SIZE];
	uint8_t sprite_line[160];
	uint8_t statInterruptTriggered;
	uint16_t windowTileAddr;
	uint16_t windowStartTileAddr;
	int clock;
	int vblankPending;
} GPU;


uint8_t gpu_rb(GPU*pgpu, uint16_t addr);
void gpu_wb(GPU*pgpu, uint16_t addr, uint8_t val);
uint8_t getPixelColor(GPU*pgpu, uint8_t pixel);

uint8_t MapSpritePixel(GPU*pgpu, uint8_t pixel, uint8_t palette_sel);

void initGPU(GPU*pgpu);
void gpu_step(GPU*pgpu, int tcycles);
void writeScanline(GPU*pgpu);
void renderScreen(GPU*pgpu);
void readOAM(GPU*pgpu);

#endif
