#include "z80.h"


#include "assert.h"




void test_BuildStatusFlag(){
	assert(buildStatusFlag(0,0,0,0) == 0);
	assert(buildStatusFlag(1,1,1,1) == 0xf0);
	assert(buildStatusFlag(0,0,0,1) == 0x10);
}


void Set_BitToVal(z80*pz80, int reg,int bit,int val);
void Test_Bit(z80*pz80,int reg,int bit);

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


void test_SetResetBit(){ // testing flags unaffected
	z80 cpu;
	initZ80(&cpu);
	cpu.registers[REGF] = 0;
	Set_Bit(&cpu,REGA,0);
	assert(cpu.registers[REGA] == 0x01);
	assert(cpu.registers[REGF] == 0);
	cpu.registers[REGF] = 0xff;
	Set_Bit(&cpu,REGA,7);
	assert(cpu.registers[REGF] ==  0xff);
	assert(cpu.registers[REGA] == 0x81);
	cpu.registers[REGF] = 0;
	Reset_Bit(&cpu,REGA,0);
	assert(cpu.registers[REGA] == 0x80);
	assert(cpu.registers[REGF] == 0);
	cpu.registers[REGF] = 0xff;
	Reset_Bit(&cpu,REGA,7);
	assert(cpu.registers[REGF] ==  0xff);
	assert(cpu.registers[REGA] == 0x00);
}


void RR_n(z80*pz80,int reg);

void test_RR_n(){

	z80 cpu;
	initZ80(&cpu);
	uint8_t flag = 0;
	
	setFlag(flag,CARRY);
	cpu.registers[REGF] = flag;
	cpu.registers[REGA] = 0x12; //00010010
	RR_n(&cpu,REGA);
	assert(cpu.registers[REGA] == 0x89); //10001001
	assert(cpu.registers[REGF] == 0x00); //0000 0000
	
	flag = 0;
	cpu.registers[REGF] = flag;
	cpu.registers[REGA] = 0x12; // 00010010
	RR_n(&cpu,REGA);
	assert(cpu.registers[REGA] == 0x09); //00001001
	assert(cpu.registers[REGF] == 0x00); //0000 0000
	
	flag = 0;
	cpu.registers[REGF] = flag;
	cpu.registers[REGA] = 0x01; // 00000001
	RR_n(&cpu,REGA);
	assert(cpu.registers[REGA] == 0x00);
	assert(cpu.registers[REGF] == 0x90); //1001 0000

}


void RRC_n(z80*pz80,int reg);

void test_RRC_n(){

	z80 cpu;
	initZ80(&cpu);
	uint8_t flag = 0;
	
	setFlag(flag,CARRY);
	cpu.registers[REGF] = flag;
	cpu.registers[REGA] = 0x12; //0001 0010
	RRC_n(&cpu,REGA);
	assert(cpu.registers[REGA] == 0x09); //0000 1001
	assert(cpu.registers[REGF] == 0x00); //0000 0000
	
	flag = 0;
	cpu.registers[REGF] = flag;
	cpu.registers[REGA] = 0x12; // 00010010
	RRC_n(&cpu,REGA);
	assert(cpu.registers[REGA] == 0x09); //0000 1001
	assert(cpu.registers[REGF] == 0x00); //0000 0000
	
	flag = 0;
	cpu.registers[REGF] = flag;
	cpu.registers[REGA] = 0x01; // 00000001
	RRC_n(&cpu,REGA);
	assert(cpu.registers[REGA] == 0x80); //1000 0000
	assert(cpu.registers[REGF] == 0x10); //0001 0000
	
	flag = 0;
	cpu.registers[REGF] = flag;
	cpu.registers[REGA] = 0x00; // 00000000
	RRC_n(&cpu,REGA);
	assert(cpu.registers[REGA] == 0x00); //0000 0000
	assert(cpu.registers[REGF] == 0x80); //1000 0000

}


void RLC_n(z80*pz80,int reg);

void test_RLC_n(){

	z80 cpu;
	initZ80(&cpu);
	uint8_t flag = 0;
	
	setFlag(flag,CARRY);
	cpu.registers[REGF] = flag;
	cpu.registers[REGA] = 0x12; //0001 0010
	RLC_n(&cpu,REGA);
	assert(cpu.registers[REGA] == 0x24); //0010 0100
	assert(cpu.registers[REGF] == 0x00); //0000 0000
	
	flag = 0;
	cpu.registers[REGF] = flag;
	cpu.registers[REGA] = 0x12; // 00010010
	RLC_n(&cpu,REGA);
	assert(cpu.registers[REGA] == 0x24); //0010 0100
	assert(cpu.registers[REGF] == 0x00); //0000 0000
	
	flag = 0;
	cpu.registers[REGF] = flag;
	cpu.registers[REGA] = 0x80; // 10000000
	RLC_n(&cpu,REGA);
	assert(cpu.registers[REGA] == 0x01); //0000 0001
	assert(cpu.registers[REGF] == 0x10); //0001 0000 carry flag
	
	flag = 0;
	cpu.registers[REGF] = flag;
	cpu.registers[REGA] = 0x00; // 00000000
	RLC_n(&cpu,REGA);
	assert(cpu.registers[REGA] == 0x00); //0000 0000
	assert(cpu.registers[REGF] == 0x80); //1000 0000 zero flag

}


void RL_n(z80*pz80,int reg);

void test_RL_n(){

	z80 cpu;
	initZ80(&cpu);
	uint8_t flag = 0;
	
	setFlag(flag,CARRY);
	cpu.registers[REGF] = flag;
	cpu.registers[REGA] = 0x12; //0001 0010
	RL_n(&cpu,REGA);
	assert(cpu.registers[REGA] == 0x25); //0010 0101
	assert(cpu.registers[REGF] == 0x00); //0000 0000
	
	flag = 0;
	cpu.registers[REGF] = flag;
	cpu.registers[REGA] = 0x12; // 00010010
	RL_n(&cpu,REGA);
	assert(cpu.registers[REGA] == 0x24); //0010 0100
	assert(cpu.registers[REGF] == 0x00); //0000 0000
	
	flag = 0;
	cpu.registers[REGF] = flag;
	cpu.registers[REGA] = 0x80; // 10000000
	RL_n(&cpu,REGA);
	assert(cpu.registers[REGA] == 0x00); //0000 0000
	assert(cpu.registers[REGF] == 0x90); //1001 0000 zero and carry flag
	
	flag = 0;
	cpu.registers[REGF] = flag;
	cpu.registers[REGA] = 0x00; // 00000000
	RL_n(&cpu,REGA);
	assert(cpu.registers[REGA] == 0x00); //0000 0000
	assert(cpu.registers[REGF] == 0x80); //1000 0000 zero flag

}


int 
main (int argc, char *argv[])
{
	test_RR_n();
	test_RRC_n();
	test_RL_n();
	test_RLC_n();
	test_BuildStatusFlag();
	test_TestBit();
	test_SetResetBit();
	return 0;
}
