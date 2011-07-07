#include "z80.h"
#include "config.h"
#include <stdio.h>
#include "debug.h"

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
	//printf("WARNING patching out bios checksum...\n");
	//wb(&(z80_cpu.mmu),0x00fa, 0x00);/*checksum bypass patch*/
	//wb(&(z80_cpu.mmu),0x00fb, 0x00);
	//wb(&(z80_cpu.mmu),0x00fc, 0x00);
	while(z80_cpu.registers16[PC] < 0x100 && Continue)
	{
		checkAndTriggerInterrupts(&z80_cpu);
		executeNextInstruction(&z80_cpu);
		ProcessInput(&Continue);
	}
	
	//printCPU(&z80_cpu);
	if(z80_cpu.registers[REGF] != 0xb0)
		printf("WARNING, possible error in bios or emulator - flags not as expected after bios\n");
			
	disableBios(&(z80_cpu.mmu));
	printf("Finished executing BIOS\n");
	/*
	z80_cpu.registers16[PC]=0x100;
	z80_cpu.registers16[SP]=0xFFFE;
	z80_cpu.registers[REGH] = 0x01;
	z80_cpu.registers[REGL] = 0x4D;
	z80_cpu.registers[REGC] = 0x13;
	z80_cpu.registers[REGE] = 0xD8;
	z80_cpu.registers[REGA] = 0x01;
	wb(&(z80_cpu.mmu),0xff40, 0x91);
	*/
	
	while(Continue){
		
		//printf("PC: %d\n", z80_cpu.registers16[PC]);
		checkAndTriggerInterrupts(&z80_cpu);
		executeNextInstruction(&z80_cpu);
		ProcessInput(&Continue);
	}

	
	return 0;
}
