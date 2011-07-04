#include "mmu.h"
#include "string.h" //memcpy bzero

uint8_t rb(MMU * pmmu,uint16_t address) {
	switch (address >> 12)
	{
		// cartridge / bios
		case 0x0: 
			if((address <= 0x00ff) && (pmmu->bios_enabled == 1))
			{
				return pmmu->bios[address];
			}
			else
			{
				return pmmu->cartridge[address];
			}
			
		// cartridge	
		case 0x1 :
		case 0x2 :
		case 0x3 :
		case 0x4 :
		case 0x5 : 
		case 0x6 :
		case 0x7 :
			return pmmu->cartridge[address];
		
		// VRAM
		case 0x8 : case 0x9 :
			// TODO: GPU
			//return gpu.vram;
			return pmmu->memory[address];
		
		// External RAM
		case 0xA : case 0xB :
			return pmmu->eram[address - 0xA000];
		
		// Working RAM
		case 0xC : case 0xD : 
			return pmmu->wram[address - 0xC000];
		
		// Working RAM shadow
		case 0xE : 
			return pmmu->wram[address - 0xE000];
			
		case 0xF :
			// Working RAM shadow
			if (address <= 0xFDFF) 
			{
				return pmmu->wram[address - 0xE000];
			}
			// OAM
			else if (address <= 0xFE9F)
			{
				// TODO: sprite information
				return pmmu->memory[address - 0xFE00];
			}
			// Memory Mapped IO
			else if (address <= 0xFF7F)
			{
				// TODO : Memory mapped IO
				return pmmu->memory[address - 0xFF00];
			}
			else
			{
				return pmmu->zram[address - 0xFF80];
			}
			
	}

	
}

void wb(MMU * pmmu,uint16_t address, uint8_t val) {
	switch (address >> 12)
	{
		// cartridge / bios
		case 0x0: 
			if((address <= 0x00ff) && (pmmu->bios_enabled == 1))
			{
				pmmu->bios[address] = val;
			}
			else
			{
				pmmu->cartridge[address] = val;
			}
			
		// cartridge	
		case 0x1 :
		case 0x2 :
		case 0x3 :
		case 0x4 :
		case 0x5 : 
		case 0x6 :
		case 0x7 :
			pmmu->cartridge[address] = val;
		
		// VRAM
		case 0x8 : case 0x9 :
			// TODO: GPU
			//return gpu.vram;
			pmmu->memory[address] = val;
		
		// External RAM
		case 0xA : case 0xB :
			pmmu->eram[address - 0xA000] = val;
		
		// Working RAM
		case 0xC : case 0xD : 
			pmmu->wram[address - 0xC000] = val;
		
		// Working RAM shadow
		case 0xE : 
			pmmu->wram[address - 0xE000] = val;
			
		case 0xF :
			// Working RAM shadow
			if (address <= 0xFDFF) 
			{
				pmmu->wram[address - 0xE000] = val;
			}
			// OAM
			else if (address <= 0xFE9F)
			{
				// TODO: sprite information
				pmmu->memory[address - 0xFE00] = val;
			}
			// Memory Mapped IO
			else if (address <= 0xFF7F)
			{
				// TODO : Memory mapped IO
				pmmu->memory[address - 0xFF00] = val;
			}
			else
			{
				pmmu->zram[address - 0xFF80] = val;
			}
			
	}
}

void ww(MMU * pmmu,uint16_t address, uint16_t val) {
	wb(pmmu,address, val & 255);
	wb(pmmu,address + 1, (val >> 8) & 255);
}

uint16_t rw(MMU * pmmu,uint16_t address) {
	return (rb(pmmu,address) + (rb(pmmu,address + 1) << 8));
}

void initMMU(MMU * pmmu)
{
	bzero((void*)pmmu,sizeof(MMU));
	uint8_t bios[BIOS_SIZE] = BIOS;
	memcpy(bios, pmmu->bios, BIOS_SIZE);
}
