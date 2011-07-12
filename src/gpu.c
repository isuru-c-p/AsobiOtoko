#include "gpu.h"
#include "SDL_Helper.h"
#include <stdio.h>

#define PRECOMPUTED_GETPIXEL_COLOUR 

void updateStat(GPU*pgpu)
{
	pgpu->STAT = (pgpu->STAT & 0xfc) | pgpu->mode;
	if(getStatInterruptEnable(pgpu, pgpu->mode + 3))
		pgpu->statInterruptTriggered = 1;
}

uint8_t gpu_rb(GPU*pgpu, uint16_t addr) {
	switch(addr)
	{
		// LCDC
		case 0xFF40:
			return pgpu->LCDC;
		// LCDC status
		case 0xFF41:
			return (pgpu->STAT & 3) | ((pgpu->LY == pgpu->LYC) << 2);
		// SCY
		case 0xFF42:
			return pgpu->SCY;
		// SCX
		case 0xFF43:
			return pgpu->SCX;
		// LY
		case 0xFF44:
			return pgpu->LY;
		// LYC
		case 0xFF45:
			return pgpu->LYC;
		// Palette
		case 0xFF47:
			return pgpu->BGP;
		// OBP0
		case 0xFF48:
			return pgpu->OBP0;
		// OBP1
		case 0xFF49:
		  return pgpu->OBP1;
		// WY
		case 0xFF4A:
		  return pgpu->WY;
		// WX
		case 0xFF4B:
		  return (pgpu->WX+7);
	}
	printf("rb : Unimplemented GPU control register: %x\n", addr);
}


void gpu_wb(GPU*pgpu, uint16_t addr, uint8_t val) {
	switch(addr)
	{
		// LCDC
		case 0xFF40:
			pgpu->LCDC = val;
			return;
		// LCDC status
		case 0xFF41:
			pgpu->STAT = val;
			return;
		// SCY
		case 0xFF42:
			pgpu->SCY = val;
			//printf("ScrollY: %d\n", pgpu->SCY);
			return;
		// SCX
		case 0xFF43:
			pgpu->SCX = val;
			return;
		// LY
		case 0xFF44:
			printf("Error! Read Only GPU address: %x\n", addr);
			return;
		// LYC
		case 0xFF45:
			pgpu->LYC = val;
			return;
		// Palette
		case 0xFF47:
			pgpu->BGP = val;
			return;
		// OBP0
		case 0xFF48:
			pgpu->OBP0 = val;
		    return;
		// OBP1
		case 0xFF49:
		    pgpu->OBP1 = val;
		    return;
		// WY
		case 0xFF4A:
		  pgpu->WY = val;
		  printf("WY: %d\n", pgpu->WY);
		  return;
		// WX
		case 0xFF4B:
		  pgpu->WX = val-7;
		  printf("WX: %d\n", pgpu->WX);
		  return;
	}
	printf("wb: Unimplemented GPU control register: %x\n", addr);
}



inline
uint8_t getPixelColor(GPU*pgpu, uint8_t pixel)
{
	#ifdef PRECOMPUTED_GETPIXEL_COLOUR
		static uint8_t lookup[256][256] = 
		#include "getPixelColourLookup.c"
		;
		return lookup[pixel][pgpu->BGP];
	#else
	uint8_t mappedPixel = (pgpu->BGP >> (2*pixel)) & 0x3;
	//if(mappedPixel != 0)
	//	printf("Pixel: %d\n", mappedPixel);
	switch(mappedPixel)
	{
		case 0:
			return 255;
		case 1:
			return 0xb6;
			//return 192;
		case 2:
			return 0x6d;
			//return 96;
		case 3:
			return 0;
		printf("Error! Mapped pixel color is out of range: %x\n", mappedPixel);
	}
	#endif
	
}

uint8_t GetColor(GPU*pgpu, uint8_t pixel)
{
	switch(pixel)
	{
		case 0:
			return 255;
		case 1:
			return 0xb6;
			//return 192;
		case 2:
			return 0x6d;
			//return 96;
		case 3:
			return 0;
		printf("Error! Mapped pixel color is out of range: %x\n", pixel);
	}
	return 0;
}

uint8_t MapSpritePixel(GPU*pgpu, uint8_t pixel, uint8_t palette_sel)
{
	uint8_t palette = 0;
	if(palette_sel == 0)
		palette = pgpu->OBP0;
	else if(palette_sel == 1)
		palette = pgpu->OBP1;
	uint8_t mappedPixel = (palette >> (2*pixel)) & 0x3;
	return mappedPixel;
}

void writeScanline(GPU*pgpu)
{
	if(!getLCDCBit(pgpu, BGWON))
	{
		memset((void*)pgpu->buffer, 0xff, sizeof(pgpu->buffer));
		return;
	}
	
	readOAM(pgpu);
	
	/*int windowEnabled = getLCDCBit(pgpu, WDISP);

	uint16_t windowTileAddr = !getLCDCBit(pgpu, BGMAP) ? 

	if(windowEnabled)
	{
		
	}*/

	int x = pgpu->SCX % 256;
	int y = (pgpu->LY + pgpu->SCY) % 256;
	int tileX = (x >> 3); // divide by 8
	int tileY = (y >> 3); // divide by 8
	uint16_t tileMapAddr = !getLCDCBit(pgpu, BGMAP) ? 0x9800 : 0x9C00;
	
	tileMapAddr += ((tileY << 5) + tileX); // tileY*32 + tileX
	int tileNo = pgpu->vram[tileMapAddr - 0x8000];
	
	if(!getLCDCBit(pgpu, BGWDATASEL) && tileNo < 128)
	{
		tileNo += 256;
	}
	
	int start_x = (x & 0x7);
	int start_y = (y & 0x7);
	uint16_t rowAddress = tileNo*16 + start_y*2;
	
	int xOffset = 0;
	
	for(xOffset = 0; xOffset < 160; xOffset++)
	{
		uint8_t pixel = (getPixel(pgpu, rowAddress + 1, (start_x+xOffset) & 0x7 ) << 1) + getPixel(pgpu, rowAddress, (start_x+xOffset) & 0x7);
		pixel = getPixelColor(pgpu, pixel);
		
		// lols CLI printout
		/*if (pixel != 255)
		{
			printf("*");
		}
		else
		{
			printf(" ");
		}*/
		//DrawPixel(xOffset, pgpu->LY, pixel);
		if(pgpu->sprite_line[xOffset] > 0x3)
		{
			pgpu->buffer[pgpu->LY*160 + xOffset] = GetColor(pgpu,(pgpu->sprite_line[xOffset] >> 1));
		}
		else if(pixel == 255)
		{
			pgpu->buffer[pgpu->LY*160 + xOffset] = GetColor(pgpu,pgpu->sprite_line[xOffset]);
		}
		else
		{
			
			pgpu->buffer[pgpu->LY*160 + xOffset] = pixel;//GetPixelVal(pixel);//pixel;
		}
		
		if(((start_x+xOffset) & 0x7) == 7)
		{
			tileMapAddr++;
			tileNo = pgpu->vram[tileMapAddr - 0x8000];
			if(!getLCDCBit(pgpu, BGWDATASEL) && tileNo < 128)
			{
				tileNo += 256;
			}
			//printf("tileNo: %d\n", tileNo);
			rowAddress = tileNo*16 + start_y*2;
			//printf("rowAddress: %d\n", rowAddress);
		}
	}
	
	// lols CLI printout
	//printf("\n");
	
	return;
}

void readOAM(GPU*pgpu)
{
	//uint8_t current_sprites[10];
	//uint8_t sprite_posX[10] = {255};
	//int nextSprite = 0;
	int i, j;
	uint8_t spriteYSize = getLCDCBit(pgpu,OBJSIZE) ? 16 : 8;
	
	bzero((void*)pgpu->sprite_line,sizeof(pgpu->sprite_line));
	
	if(!getLCDCBit(pgpu, OBJON))
	{
		return;
	}
	
	for(i = 0; i < 40; i++)
	{
		uint8_t spriteX = (pgpu->oam[(i*4)+1]) - 8;//getSpriteX(pgpu, i) - 8;
		uint8_t spriteY = (pgpu->oam[(i*4)]) - 16;//getSpriteY(pgpu, i) - 16;
		
		//if((spriteX == -8) && (spriteY == -8))
		//	continue;
		
		//if(i == 39)
		//	printf("oam[%d], LY: %d, SpriteX: %d, SpriteY: %d, SpriteYSize:%d\n", i*4, pgpu->LY, spriteX, spriteY, spriteYSize);
			
		if((pgpu->LY >= spriteY) && (pgpu->LY <= (spriteY + spriteYSize)))
		{
			//printf("i: %d, LY: %d, Sprite at: %d,%d\n", i, pgpu->LY, spriteX, spriteY);
			uint8_t sprite_no = getLCDCBit(pgpu,OBJSIZE) ? (getSpriteTile(pgpu, i) & 0xfe) : getSpriteTile(pgpu, i);
			uint16_t sprite_addr;
			
			if(getSpriteAttr(pgpu, i, YFLIP))
			{
				sprite_addr = ((uint16_t)sprite_no * spriteYSize * 2) + ((7 - (pgpu->LY % spriteYSize)) * 2);
			}
			else
			{
				sprite_addr = ((uint16_t)sprite_no * spriteYSize * 2) + ((pgpu->LY % spriteYSize) * 2);
			}
			
			for(j= spriteX; j <= (spriteX+8); j++)
			{
				if(pgpu->sprite_line[j] == 0)
				{
					if(!getSpriteAttr(pgpu, i, XFLIP))
					{
						pgpu->sprite_line[j] = ((pgpu->vram[sprite_addr] >> (7-(j-spriteX))) & 0x1) + (((pgpu->vram[sprite_addr+1] >> (7-(j-spriteX))) & 0x1) << 1);
					}
					else
					{
						pgpu->sprite_line[j] = ((pgpu->vram[sprite_addr] >> (j-spriteX)) & 0x1) + (((pgpu->vram[sprite_addr+1] >> (j-spriteX)) & 0x1) << 1);
					}
					pgpu->sprite_line[j] = MapSpritePixel(pgpu, pgpu->sprite_line[j], getSpriteAttr(pgpu, i, SPRITE_PALETTE));
					
					if(!getSpriteAttr(pgpu, i, PRIORITY))
					{
						pgpu->sprite_line[j] = (pgpu->sprite_line[j] << 1);
					}
					//printf("Sprite Line(%d): %x\n", j, pgpu->sprite_line[j]);
				
				}
				
				// TODO: implement sprite priority wrt to x pos
				// (sprites with smaller x positions have higher priority)
			}
		}
		else
		{
			;
			//printf("Sprite not being drawn. LY: %d	 i: %d, SpriteX: %d, SpriteY: %d\n", i, spriteX, spriteY);
		}
	}
	
	return;
}

void renderScreen(GPU*pgpu)
{
	Flip(pgpu->buffer);
	RenderScreen();
	return;
}

void gpu_step(GPU*pgpu, int tcycles)
{
	//printf("GPU clock: %d\n", pgpu->clock);
	pgpu->clock += tcycles;
	
	switch(pgpu->mode)
	{
		// OAM access 
		case 2:
			if(pgpu->clock >= 80)
			{
				pgpu->mode = 3;
				updateStat(pgpu);
				pgpu->clock = 0;
			}
			break;
		// VRAM access 
		case 3:
			if(pgpu->clock >= 172)
			{
				pgpu->mode = 0;
				updateStat(pgpu);
				pgpu->clock = 0;
				pgpu->LY++;
				//printf("LY: %d\n", pgpu->LY);
				writeScanline(pgpu);
			}
			break;
		// HBlank
		case 0:
			if(pgpu->clock >= 204)
			{
				if(pgpu->LY == 143)
				{
					renderScreen(pgpu);
					pgpu->mode = 1;
					pgpu->vblankPending = 1;
					updateStat(pgpu);
				}
				else
				{
					pgpu->mode = 2;
					updateStat(pgpu);
				}
			}
			break;
		// VBlank
		case 1:
			if(pgpu->clock >= 456)
			{
				pgpu->LY++;
				//printf("LY: %d\n", pgpu->LY);
				pgpu->clock = 0;
				
				if(pgpu->LY == 153)
				{
					pgpu->LY = 0;
					pgpu->mode = 3;
					updateStat(pgpu);
				}	
			}
			break;
	}
	
	if(getStatInterruptEnable(pgpu, LYCLY_INT) && (pgpu->LY == pgpu->LYC))
	{
		pgpu->statInterruptTriggered = 1;
	}
}

void initGPU(GPU*pgpu)
{
	if(!init_graphics())
	{
		printf("Init graphics failed.\n");
	}
	bzero((void*)pgpu,sizeof(pgpu));
	return;
}
