#include "mmu.h"
#include "string.h" //memcpy bzero
#include "assert.h"
#include "io.h"
#include <stdio.h>


#ifdef USE_ADDRESS_LUT
void fillAddressLUTEntry(MMU * pmmu,uint16_t address) {
	
	switch (address >> 12)
	{
		// cartridge / bios
		case 0x0: 
			if((address <= 0x00ff) && (pmmu->bios_enabled == 1))
			{
				pmmu->addressLUT[address] = &(pmmu->bios[address]);
				return;
			}
			else
			{
				pmmu->addressLUT[address] = &(pmmu->cartridge[address]);
				return;
			}
			break;
			
		// cartridge	
		case 0x1 :
		case 0x2 :
		case 0x3 :
		case 0x4 :
		case 0x5 : 
		case 0x6 :
		case 0x7 :
			pmmu->addressLUT[address] = &(pmmu->cartridge[address]);
			return;
		
		// VRAM
		case 0x8 : case 0x9 :
			pmmu->addressLUT[address] = &(pmmu->gpu.vram[address - 0x8000]);
			return;
		
		// External RAM
		case 0xA : case 0xB :
			pmmu->addressLUT[address] = &(pmmu->eram[address - 0xA000]);
			return;
		
		// Working RAM
		case 0xC : case 0xD : 
			pmmu->addressLUT[address] = &(pmmu->wram[address - 0xC000]);
			return;
		
		// Working RAM shadow
		case 0xE : 
			pmmu->addressLUT[address] = &(pmmu->wram[address - 0xE000]);
			return;
			
		case 0xF :
			// Working RAM shadow
			if (address <= 0xFDFF) 
			{
				pmmu->addressLUT[address] = &(pmmu->wram[address - 0xE000]);
				return;
			}
			// OAM
			else if (address <= 0xFE9F)
			{
				// TODO: sprite information
				pmmu->addressLUT[address] = &(pmmu->gpu.oam[address - 0xFE00]);
				return;
			}
			// Memory Mapped IO
			else if (address <= 0xFF7F)
			{
				// TODO : Memory mapped IO
				if(address == 0xff00)
				{
					pmmu->addressLUT[address] = 0;
					return;
				}
				else if((address & 0xf0) == 0x40)
				{
					pmmu->addressLUT[address] = 0;
					return;
				}
				pmmu->addressLUT[address] = &(pmmu->memory[address]);
				return;
			}
			else
			{
				pmmu->addressLUT[address] = &(pmmu->zram[address - 0xFF80]);
				return;
			}
			break;
			
	}

	assert(0); // Should never execute.
	return;
}

void fillAddressLUT(MMU*pmmu){
	uint16_t address = 0;
	do{
		fillAddressLUTEntry(pmmu,address);
		address++;
	}while(address); //Loop until a wrap around.
}

#endif


uint8_t rb(MMU * pmmu,uint16_t address) {
	#ifdef USE_ADDRESS_LUT
	//if((address >= 0xFE00) && (address <= 0xFE9F))
	//	printf("OAM Read: %x\n", address);
	
	uint8_t * b = pmmu->addressLUT[address];
	if(b)
		return *b;
	if(address == 0xff00) // exceptions are dynamic
	{
		return readP1();
	}
	else if((address & 0xf0) == 0x40) // might be able to precompute
	{
		return gpu_rb(&(pmmu->gpu), address);
	}
	#else
	switch (address >> 12)
	{
		// cartridge / bios
		case 0x0:
		case 0x1 : 
		case 0x2 :
		case 0x3 :
			if((address <= 0x00ff) && (pmmu->bios_enabled == 1))
			{
				return pmmu->bios[address];
			}
			else
			{
				return pmmu->cartridge[address];
			}
			break;
			
		// cartridge	
		case 0x4 :
		case 0x5 : 
		case 0x6 :
		case 0x7 :
			if((pmmu->rom_type != ROM_ONLY))
			{
				if(pmmu->rom_bank)
				{
					return pmmu->cartridge[(pmmu->rom_bank*0x4000)+(address - 0x4000)];
				}
				else
				{
					return pmmu->cartridge[address - 0x4000];
				}
			}
			else
				return pmmu->cartridge[address];
			break;
		
		// VRAM
		case 0x8 : case 0x9 :
			// TODO: GPU
			//return gpu.vram;
			return pmmu->gpu.vram[address - 0x8000];
			break;
		
		// External RAM
		case 0xA : case 0xB :
			//if(pmmu->ram_bank_enable)
			//{	
				return pmmu->eram[(pmmu->ram_bank*0x4000) + (address - 0xA000)];
			/*}
			else
			{
				printf("Read Error! External RAM bank not enabled.\n");
				return 0;
			}*/
			break;
		
		// Working RAM
		case 0xC : case 0xD : 
			return pmmu->wram[address - 0xC000];
			break;
		
		// Working RAM shadow
		case 0xE : 
			return pmmu->wram[address - 0xE000];
			break;
			
		case 0xF :
			// Working RAM shadow
			if (address <= 0xFDFF) 
			{
				return pmmu->wram[address - 0xE000];
			}
			// OAM
			else if (address <= 0xFE9F)
			{
				return pmmu->gpu.oam[address - 0xFE00];
			}
			// Memory Mapped IO
			else if (address <= 0xFF7F)
			{
				// TODO : Memory mapped IO
				if(address == 0xff00)
				{
					return readP1();
				}
				else if((address & 0xf0) == 0x40)
				{
					return gpu_rb(&(pmmu->gpu), address);
				}
				return pmmu->memory[address];
			}
			else
			{
				//if(address == 0xffff)
				//	printf("Reading IE: %x\n", pmmu->zram[address - 0xFF80]);
				return pmmu->zram[address - 0xFF80];
			}
			break;
			
	}

	assert(0); // should never execute
	return 0;
	#endif
}

void wb(MMU * pmmu,uint16_t address, uint8_t val) {
	//printf("Address: %x, Val: %x\n", address, val);
	#ifdef DEBUG
		if(address == 0xFFFF)
			printf("New IE: %x\n", address);
	#endif
	
	switch (address >> 12)
	{
		// cartridge / bios
		case 0x0: 
		case 0x1 :
			if(pmmu->rom_type != ROM_ONLY)
			{
				if(val == 0xa)
				{
					pmmu->ram_bank_enable = 1;
				}
				else
				{
					pmmu->ram_bank_enable = 0;
				}
				
				#ifdef DEBUG
					printf("Setting ext RAM enable: %d (%d)\n", pmmu->ram_bank_enable, val);
				#endif
				return;
			}
			printf("ERROR1! Attempting to write to ROM. Address: %x, Val: %x\n", address, val);
			break;
			
		
		case 0x2 :
		case 0x3 :
			if(pmmu->rom_type == MBC1)
			{
				uint8_t newVal = ( val & 0x1f );
				
				if( !newVal )
				{
					newVal = 1;
				}
				
				pmmu->rom_bank = ( pmmu->rom_bank & 0xc0 ) | ( newVal );
				
				#ifdef DEBUG
					printf("Selecting ROM bank: %d (val: %d, newVal:%d)\n", pmmu->rom_bank, val, newVal);
				#endif
	
				return;
			}
			else if (pmmu->rom_type == MBC3)
			{
				pmmu->rom_bank = (val & 0x7f);
				return;
			}
			
			printf("ERROR1! Attempting to write to ROM. Address: %x, Val: %x\n", address, val);
			return;
		
		case 0x4 :
		case 0x5 : 
			if((pmmu->rom_type == MBC1) && (pmmu->mbc1_mode == MBC1_4_32_MODE))
			{
				pmmu->ram_bank = val & 0x3;
				#ifdef DEBUG
					printf("Selecting RAM bank: %d\n", pmmu->ram_bank);
				#endif
				return;
			}
			else if((pmmu->rom_type == MBC1) && (pmmu->mbc1_mode == MBC1_16_8_MODE))
			{
				pmmu->rom_bank = (pmmu->rom_bank & 0x3f) || (val << 6);
				#ifdef DEBUG
					printf("Selecting Rom bank: %d\n", pmmu->rom_bank);
				#endif
				return;
			}
			printf("ERROR1! Attempting to write to ROM. Address: %x, Val: %x\n", address, val);
			return;
		
		case 0x6 :
		case 0x7 :
			if(pmmu->rom_type == MBC1)
			{
				pmmu->mbc1_mode = val & 0x01;
				#ifdef DEBUG
					printf("Setting MBC1 Mode to: %d\n",pmmu->mbc1_mode);
				#endif
				return;
			}
			//pmmu->cartridge[address] = val;
			printf("ERROR1! Attempting to write to ROM. Address: %x, Val: %x\n", address, val);
			return;
			
		// VRAM
		case 0x8 : case 0x9 :
			// TODO: GPU
			//return gpu.vram;
			/*if(address < 0x9800)
				printf("Tiledata update: %x, val: %d\n", address, val);
			if(address >= 0x9800)
				printf("Tilemap update: %x, val: %d\n", address, val);*/
			pmmu->gpu.vram[address - 0x8000] = val;
			break;
		
		// External RAM
		case 0xA : case 0xB :
			//if(pmmu->ram_bank_enable)
			//{
				pmmu->eram[(pmmu->ram_bank*0x4000) + (address - 0xA000)] = val;
			/*}
			else
			{
				printf("Write Error! RAM Bank is not enabled.\n");
			}*/
			break;
		
		// Working RAM
		case 0xC : case 0xD : 
			pmmu->wram[address - 0xC000] = val;
			break;
		
		// Working RAM shadow
		case 0xE : 
			pmmu->wram[address - 0xE000] = val;
			break;
			
		case 0xF :
			// Working RAM shadow
			if (address <= 0xFDFF) 
			{
				pmmu->wram[address - 0xE000] = val;
				return;
			}
			// OAM
			else if (address <= 0xFE9F)
			{
				// TODO: sprite information
				//printf("OAM access! address: %x\n", address);
				pmmu->gpu.oam[address - 0xFE00] = val;
				return;
			}
			// Memory Mapped IO
			else if (address <= 0xFF7F)
			{
				// TODO : Memory mapped IO
				if(address == 0xff00)
				{
					writeP1(val);
					return;
				}
				else if(address == 0xff46)
				{
					//printf("Initiating DMA transfer of OAM from: %x\n", (val<<8));
					int i;
					for(i = 0; i < 160; i++)
					{
						uint8_t byte = rb(pmmu, (val<<8)+i); 
						//printf("Transferring: oam[%d] = %x\n", i, byte);
						pmmu->gpu.oam[i] = byte;
					}
					#ifdef DEBUG
					//logging_enabled = 1;
					#endif
					//memcpy(pmmu->gpu.oam, &(pmmu->memory[val]), 160);
					return;
				}
				else if((address & 0xf0) == 0x40)
				{
					gpu_wb(&(pmmu->gpu), address, val);
					return;
				}
				else if(address == 0xff04)
				{
					pmmu->memory[address] = 0;
					return;
				}
				
				pmmu->memory[address] = val;
			}
			else
			{
				pmmu->zram[address - 0xFF80] = val;
			}
			break;
			
	}
}

void ww(MMU * pmmu,uint16_t address, uint16_t val) {
	wb(pmmu,address, val & 255);
	wb(pmmu,address + 1, (val >> 8) & 255);
}

uint16_t rw(MMU * pmmu,uint16_t address) {
	return (rb(pmmu,address) + (rb(pmmu,address + 1) << 8));
}


void
disableBios(MMU*pmmu){
	pmmu->bios_enabled = 0;
	#ifdef USE_ADDRESS_LUT
	fillAddressLUT(pmmu);
	#endif
}


void initMMU(MMU * pmmu)
{
	bzero((void*)pmmu,sizeof(MMU));
	uint8_t bios[BIOS_SIZE] = __BIOS__ ;
	memcpy(pmmu->bios, bios, BIOS_SIZE);
	pmmu->bios_enabled = 1;
	pmmu->rom_bank = 1;
	pmmu->ram_bank_enable = 1;
	initGPU(&(pmmu->gpu));
	#ifdef USE_ADDRESS_LUT
	fillAddressLUT(pmmu);
	#endif
}
