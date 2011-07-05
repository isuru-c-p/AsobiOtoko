#include "z80.h"
#include <stdio.h>

int loadROM(z80* pz80, char* path)
{
	FILE* fp;
	fp = fopen(path, "r");
	int byte;
	int byteNo = 0;
	
	for(byteNo = 0; (byte = fgetc(fp)) != EOF; byteNo++)
	{
		pz80->mmu.cartridge[byteNo] = byte;
	}
	
	fclose(fp);
}

int
main (int argc, char *argv[]){
	if(argc != 2)
	{
		printf("Usage: ./AsobiOtoko [path to ROM]\n");
		return 0;
	}
	z80 pz80;
	initZ80(&pz80);
	
	loadROM(&pz80, argv[1]);
	
	
	while(pz80.registers16[PC] < 0x100)
	{
		//printf("PC: %d\n", pz80.registers16[PC]);
		executeNextInstruction(&pz80);
	}
	
	printf("Finished executing BIOS\n");
	pz80.mmu.bios_enabled = 0;
	
	for(;;)
	{
		//printf("PC: %d\n", pz80.registers16[PC]);
		executeNextInstruction(&pz80);
	}
	
	return 0;
}
