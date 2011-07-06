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
	z80 pz80;
	initZ80(&pz80);
	
	if(loadROM(&pz80, argv[1]) == -1){
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
	pz80.mmu.bios_enabled = 0;
	pz80.registers16[PC]=0x100;
	pz80.registers16[SP]=0xFFFE;
	pz80.registers[REGH] = 0x01;
	pz80.registers[REGL] = 0x4D;
	pz80.registers[REGC] = 0x13;
	pz80.registers[REGE] = 0xD8;
	pz80.registers[REGA] = 0x01;
	wb(&(pz80.mmu),0xff40, 0x91);
	
	
	while(Continue){
		
		//printf("PC: %d\n", pz80.registers16[PC]);
		executeNextInstruction(&pz80);
		ProcessInput(&pz80,&Continue);
	}

	
	return 0;
}
