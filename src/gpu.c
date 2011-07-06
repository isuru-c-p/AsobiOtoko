#include "gpu.h"
#include "SDL_Helper.h"
#include <stdio.h>

uint8_t gpu_rb(GPU*pgpu, uint16_t addr) {
	switch(addr)
	{
		// LCDC
		case 0xFF40:
			return pgpu->LCDC;
		// LCDC status
		case 0xFF41:
			return pgpu->STAT;
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
		//case 0xFF48:
		//	return pgpu->OBP0;
		// OBP1
		//case 0xFF49:
		//  return pgpu->OBP1;
		// WY
		//case 0xFF4A:
		//  return pgpu->WY;
		// WX
		//case 0xFF4B:
		//  return pgpu->WX;
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
		//case 0xFF48:
		//	pgpu->OBP0 = val;
		//  return;
		// OBP1
		//case 0xFF49:
		//  pgpu->OBP1 = val;
		//  return;
		// WY
		//case 0xFF4A:
		//  pgpu->WY = val;
		//  return;
		// WX
		//case 0xFF4B:
		//  pgpu->WX = val;
		//  return;
	}
	printf("wb: Unimplemented GPU control register: %x\n", addr);
}

uint8_t getPixelColor(GPU*pgpu, uint8_t pixel)
{
	uint8_t mappedPixel = (pgpu->BGP >> (2*pixel)) & 0x3;
	//if(mappedPixel != 0)
	//	printf("Pixel: %d\n", mappedPixel);
	switch(mappedPixel)
	{
		case 0:
			return 255;
		case 1:
			return 192;
		case 2:
			return 96;
		case 3:
			return 0;
	}
	
	printf("Error! Mapped pixel color is out of range: %x\n", mappedPixel);
}

void writeScanline(GPU*pgpu)
{
	if(!getLCDCBit(pgpu, BGWON))
		return;

	int x = pgpu->SCX;
	int y = pgpu->LY + pgpu->SCY;
	int tileX = (x >> 3); // divide by 8
	int tileY = (y >> 3); // divide by 8
	uint16_t tileMapAddr = !getLCDCBit(pgpu, BGMAP) ? 0x9800 : 0x9C00;
	
	tileMapAddr += ((tileY << 5) + tileX); // tileY*32 + tileX
	//printf("TileMapAddr: %x\n", tileMapAddr);
	int tileNo = pgpu->vram[tileMapAddr - 0x8000];
	
	if(!getLCDCBit(pgpu, BGWDATASEL) && tileNo < 127)
	{
		tileNo += 256;
	}
	//printf("tileNo: %d\n", tileNo);
	
	int start_x = (x % 8);
	int start_y = (y % 8);
	uint16_t rowAddress = tileNo*16 + start_y*2;
	//printf("rowAddress: %x\n", 0x8000+rowAddress);
	
	int xOffset = 0;
	
	for(xOffset = 0; xOffset < 160; xOffset++)
	{
		uint8_t pixel = (getPixel(pgpu, rowAddress + 1, (start_x+xOffset)%8 ) << 1) + getPixel(pgpu, rowAddress, (start_x+xOffset)%8);
		pixel = getPixelColor(pgpu, pixel);
		//if (pixel != 255)
		//	printf("Pixel*: %d\n", pixel);
		
		// lols CLI printout
		/*if (pixel != 255)
		{
			printf("*");
		}
		else
		{
			printf(" ");
		}*/
		DrawPixel(xOffset, pgpu->LY, pixel);
		
		if(((start_x+xOffset) % 8) == 7)
		{
			tileMapAddr++;
			tileNo = pgpu->vram[tileMapAddr - 0x8000];
			if(!getLCDCBit(pgpu, BGWDATASEL) && tileNo < 127)
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

void renderScreen(GPU*pgpu)
{
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
				pgpu->clock = 0;
			}
			break;
		// VRAM access 
		case 3:
			if(pgpu->clock >= 172)
			{
				pgpu->mode = 0;
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
				}
				else
				{
					pgpu->mode = 2;
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
				}	
			}
			break;
	}
}

void initGPU(GPU*pgpu)
{
	if(!init_graphics())
	{
		printf("Init graphics failed.\n");
	}
	return;
}