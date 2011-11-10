#include "z80.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "SDL_Helper.h"

int loadROM(z80* pz80, char* path)
{
	FILE* fp;
	fp = fopen(path, "r");
	if(!fp)
		return -1;
		
	fseek(fp, 0L, SEEK_END);
	int fileSize = ftell(fp);	
	fseek(fp, 0L, SEEK_SET);
	pz80->mmu.cartridge = (uint8_t*)malloc(fileSize);
	int byte;
	int byteNo = 0;
	
	for(byteNo = 0; (byte = fgetc(fp)) != EOF; byteNo++)
	{
		pz80->mmu.cartridge[byteNo] = byte;
	}
	
	switch(pz80->mmu.cartridge[0x0147])
	{
		case 1:
		case 2:
		case 3:
			pz80->mmu.rom_type = MBC1;
			printf("ROM type: MBC1\n");
			break;
			
		case 0x13:
			printf("ROM type: MBC3\n");
			pz80->mmu.rom_type = MBC3;
			break;
		
		default:
			printf("ROM type signature: %x\n", pz80->mmu.cartridge[0x0147]);
			pz80->mmu.rom_type = ROM_ONLY;
			printf("ROM type: ROM_ONLY\n");
			break;
	}
	
	fclose(fp);
	return 0;
}

int
parseCommandLine(int argc, char ** argv){
	/* TODO fill in global config vars based on parsing */
	return argc == 2 ? 0 : -1;
}

int
main (int argc, char *argv[]){

	
	if(parseCommandLine(argc,argv) == -1)
	{
		puts(PACKAGE_STRING);
		printf("Usage: %s [path to ROM]\n",argv[0]);
		return 0;
	}
	z80 z80_cpu;
	initZ80(&z80_cpu);
	
	if(loadROM(&z80_cpu, argv[1]) == -1){
		printf("Error loading rom %s\n",argv[1]);
		return 1;
	}
	
	int Continue = 1;

	/*while(z80_cpu.registers16[PC] < 0x100 && Continue)
	{
		checkAndTriggerInterrupts(&z80_cpu);
		executeNextInstruction(&z80_cpu);
		ProcessInput(&Continue);
	}
	
	//printCPU(&z80_cpu);
	if(z80_cpu.registers[REGF] != 0xb0)
		printf("WARNING, possible error in bios or emulator - flags not as expected after bios\n");
	*/
	disableBios(&(z80_cpu.mmu));

	printf("Finished executing BIOS\n");
	
	z80_cpu.registers16[PC]=0x100;
	z80_cpu.registers16[SP]=0xFFFE;
	z80_cpu.registers[REGH] = 0x01;
	z80_cpu.registers[REGL] = 0x4D;
	z80_cpu.registers[REGC] = 0x13;
	z80_cpu.registers[REGE] = 0xD8;
	z80_cpu.registers[REGA] = 0x01;
	wb(&(z80_cpu.mmu),0xff40, 0x91);
	
	
	while(Continue){
		
		//printf("PC: %d\n", z80_cpu.registers16[PC]);
		checkAndTriggerInterrupts(&z80_cpu);
		executeNextInstruction(&z80_cpu);
		updateCPUTime(&z80_cpu);
		ProcessInput(&Continue);
		if(button_irq)
		{
			#ifdef DEBUG
				printf("Button interrupt pending.\n");
			#endif
			setInterruptPending(&z80_cpu,P0_P13_INT);
			button_irq = 0;
		}
	}
	
	free(z80_cpu.mmu.cartridge);

	
	return 0;
}
