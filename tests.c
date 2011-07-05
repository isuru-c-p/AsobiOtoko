#include "z80.h"


#include "assert.h"


void test_BuildStatusFlag(){
	assert(buildStatusFlag(0,0,0,0) == 0);
	assert(buildStatusFlag(1,1,1,1) == 0xf0);
	assert(buildStatusFlag(0,0,0,1) == 0x10);
}

void test_TestBit(){
	z80 cpu;
	initZ80(&cpu);
	cpu.registers[REGA] = 1;
	Test_Bit(&cpu,REGA,0);

	assert(!getFlag(cpu.registers[REGF],ZERO));
	assert(!getFlag(cpu.registers[REGF],CARRY));
	assert(getFlag(cpu.registers[REGF],HALFCARRY));
	assert(!getFlag(cpu.registers[REGF],SUB));
	
	setFlag(cpu.registers[REGF],CARRY); // carry should not change
	assert(!getFlag(cpu.registers[REGF],ZERO));
	assert(getFlag(cpu.registers[REGF],CARRY));
	assert(getFlag(cpu.registers[REGF],HALFCARRY));
	assert(!getFlag(cpu.registers[REGF],SUB));
	
	Test_Bit(&cpu,REGA,1);
	assert(getFlag(cpu.registers[REGF],ZERO));
	assert(getFlag(cpu.registers[REGF],CARRY));
	assert(getFlag(cpu.registers[REGF],HALFCARRY));
	assert(!getFlag(cpu.registers[REGF],SUB));
	
	
}



int 
main (int argc, char *argv[])
{
	test_BuildStatusFlag();
	test_TestBit();
	return 0;
}
