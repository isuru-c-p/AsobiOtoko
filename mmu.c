#include "mmu.h"
#include "strings.h" //bzero

uint8_t rb(int address) {
	switch (address >> 12)
	{
		// cartridge / bios
		case 0x0: 
			if((address <= 0x00ff) && (mmu.bios_enabled == 1))
			{
				return mmu.bios[address];
			}
			else
			{
				return mmu.cartridge[address];
			}
			
		// cartridge	
		case 0x1 :
		case 0x2 :
		case 0x3 :
		case 0x4 :
		case 0x5 : 
		case 0x6 :
		case 0x7 :
			return mmu.cartridge[address];
		
		// VRAM
		case 0x8 : case 0x9 :
			// TODO: GPU
			//return gpu.vram;
			return mmu.memory[address];
		
		// External RAM
		case 0xA : case 0xB :
			return mmu.eram[address - 0xA000];
		
		// Working RAM
		case 0xC : case 0xD : 
			return mmu.wram[address - 0xC000];
		
		// Working RAM shadow
		case 0xE : 
			return mmu.wram[address - 0xE000];
			
		case 0xF :
			// Working RAM shadow
			if (address <= 0xFDFF) 
			{
				return mmu.wram[address - 0xE000];
			}
			// OAM
			else if (address <= 0xFE9F)
			{
				// TODO: sprite information
				return mmu.memory[address - 0xFE00];
			}
			// Memory Mapped IO
			else if (address <= 0xFF7F)
			{
				// TODO : Memory mapped IO
				return mmu.memory[address - 0xFF00];
			}
			else
			{
				return mmu.zram[address - 0xFF80];
			}
			
	}

	
}

void wb(int address, int val) {
	switch (address >> 12)
	{
		// cartridge / bios
		case 0x0: 
			if((address <= 0x00ff) && (mmu.bios_enabled == 1))
			{
				mmu.bios[address] = val;
			}
			else
			{
				mmu.cartridge[address] = val;
			}
			
		// cartridge	
		case 0x1 :
		case 0x2 :
		case 0x3 :
		case 0x4 :
		case 0x5 : 
		case 0x6 :
		case 0x7 :
			mmu.cartridge[address] = val;
		
		// VRAM
		case 0x8 : case 0x9 :
			// TODO: GPU
			//return gpu.vram;
			mmu.memory[address] = val;
		
		// External RAM
		case 0xA : case 0xB :
			mmu.eram[address - 0xA000] = val;
		
		// Working RAM
		case 0xC : case 0xD : 
			mmu.wram[address - 0xC000] = val;
		
		// Working RAM shadow
		case 0xE : 
			mmu.wram[address - 0xE000] = val;
			
		case 0xF :
			// Working RAM shadow
			if (address <= 0xFDFF) 
			{
				mmu.wram[address - 0xE000] = val;
			}
			// OAM
			else if (address <= 0xFE9F)
			{
				// TODO: sprite information
				mmu.memory[address - 0xFE00] = val;
			}
			// Memory Mapped IO
			else if (address <= 0xFF7F)
			{
				// TODO : Memory mapped IO
				mmu.memory[address - 0xFF00] = val;
			}
			else
			{
				mmu.zram[address - 0xFF80] = val;
			}
			
	}
}

void ww(int address, int val) {
	wb(address, val & 255);
	wb(address + 1, (val >> 8) & 255);
}

uint16_t rw(int address) {
	return (rb(address) + (rb(address + 1) << 8));
}

void initMMU()
{
	bzero((void*)&mmu,sizeof(MMU));
	uint8_t bios[BIOS_SIZE] = BIOS;
	memcpy(bios, mmu.bios, BIOS_SIZE);
}