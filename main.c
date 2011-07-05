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
	return 0;
}
