#include "z80.h"
#include "config.h"
#include <stdio.h>
#include "SDL/SDL.h"

int loadROM(z80* pz80, char* path)
{
	FILE* fp;
	fp = fopen(path, "r");
	if(!fp)
		return -1;
	int byte;
	int byteNo = 0;
	
	for(byteNo = 0; (byte = fgetc(fp)) != EOF; byteNo++)
	{
		pz80->mmu.cartridge[byteNo] = byte;
	}
	
	fclose(fp);
	return 0;
}

/*TODO probably gonna need to be made more portable etc*/
void ProcessInput(z80*pz80,int * cont){
		static	SDL_Event event;
		SDL_PollEvent(&event);
		switch(event.type){
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				break;
			case SDL_QUIT:
				*cont = 0;
				break;
		}
}

int
main (int argc, char *argv[]){
	if(argc != 2)
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
	/*while(pz80.registers16[PC] < 0x100 && Continue)
	{
		//printf("PC: %d\n", pz80.registers16[PC]);
		executeNextInstruction(&pz80);
		ProcessInput(&pz80,&Continue);
	}
	
	printf("Finished executing BIOS\n");*/
	z80_cpu.mmu.bios_enabled = 0;
	z80_cpu.registers16[PC]=0x100;
	z80_cpu.registers16[SP]=0xFFFE;
	z80_cpu.registers[REGH] = 0x01;
	z80_cpu.registers[REGL] = 0x4D;
	z80_cpu.registers[REGC] = 0x13;
	z80_cpu.registers[REGE] = 0xD8;
	z80_cpu.registers[REGA] = 0x01;
	wb(&(z80_cpu.mmu),0xff40, 0x91);
	
	
	while(Continue){
		
		//printf("PC: %d\n", pz80.registers16[PC]);
		checkAndTriggerInterrupts(&z80_cpu);
		executeNextInstruction(&z80_cpu);
		ProcessInput(&z80_cpu,&Continue);
	}

	
	return 0;
}
