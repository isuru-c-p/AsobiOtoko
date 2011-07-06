#include "z80.h"
#include "mmu.h"

#include "string.h" //bzero
#include <stdio.h>


uint8_t 
buildStatusFlag(int zero, int sub, int halfcarry,int carry){
	uint8_t ret = 0;
	if(zero)
		setFlag(ret,ZERO);
	if(sub)
		setFlag(ret,SUB);
	if(halfcarry)
		setFlag(ret,HALFCARRY);
	if(carry)
		setFlag(ret,CARRY);
	return ret;
}

void 
initZ80(z80*pz80){
	bzero((void*)pz80,sizeof(z80));
	initMMU(&(pz80->mmu));
}

void loadRegMemFromHL(z80*pz80){
	uint16_t addr = getRegister16(pz80,REGH,REGL);
	pz80->registers[REGMEM] = rb(&(pz80->mmu),addr);
}

void 
saveRegMemToHL(z80*pz80){
	uint16_t addr = getRegister16(pz80,REGH,REGL);
	wb(&(pz80->mmu),addr,pz80->registers[REGMEM]);
}


void INC_n(z80*pz80, int reg)
{
	pz80->registers[reg]++;
	pz80->registers[REGF] = buildStatusFlag((pz80->registers[reg]==0), 0, ((pz80->registers[reg] & 0xff) == 0), getFlag(pz80->registers[REGF], CARRY));
	pz80->tcycles = 4;
	incPC(pz80, 1);
}

void INC_nn(z80*pz80, int h, int l)
{
	if(pz80->registers[l] == 0xff)
		pz80->registers[h]++;
	pz80->registers[l]++;
	pz80->registers[REGF] = buildStatusFlag((pz80->registers[h]==0) && (pz80->registers[l]==0), 0, (pz80->registers[l] == 0), getFlag(pz80->registers[REGF], CARRY));
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void DEC_n(z80*pz80, int reg)
{
	pz80->registers[reg]--;
	pz80->registers[REGF] = buildStatusFlag((pz80->registers[reg]==0), 1, ((pz80->registers[reg] & 0xff) == 0xff), getFlag(pz80->registers[REGF], CARRY));
	pz80->tcycles = 4;
	incPC(pz80, 1);
	#ifdef DEBUG
		printf("DEC_n, Register: %d, Result: %d ",reg, pz80->registers[reg]);
	#endif
}

void DEC_nn(z80*pz80, int h, int l)
{
	if(pz80->registers[l] == 0x0)
		pz80->registers[h]--;
	pz80->registers[l]--;
	pz80->registers[REGF] = buildStatusFlag((pz80->registers[h]==0) && (pz80->registers[l]==0), 1, (pz80->registers[l] == 0xff), getFlag(pz80->registers[REGF], CARRY));
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void ADC_A_n(z80*pz80, int src)
{
	uint8_t carry = getFlag(pz80->registers[REGF], CARRY);
	pz80->registers[REGF] = buildStatusFlag(((pz80->registers[REGA] + pz80->registers[src] + carry)==0), 0, (((pz80->registers[REGA] & 0xff) + (pz80->registers[src] & 0xff) + carry) > 0xff), (pz80->registers[REGA] + pz80->registers[src] + carry) > 0xffff);
	pz80->registers[REGA] = pz80->registers[REGA] + pz80->registers[src] + carry;
	pz80->tcycles = 4;
	incPC(pz80, 1);
}

void ADC_A_nn_mem(z80*pz80, int srcH, int srcL)
{
	uint8_t carry = getFlag(pz80->registers[REGF], CARRY);
	uint8_t val = rb(&(pz80->mmu), (pz80->registers[srcH] << 8) + pz80->registers[srcL]);
	pz80->registers[REGF] = buildStatusFlag(((pz80->registers[REGA] + val + carry)==0), 0, (((pz80->registers[REGA] & 0xff) + (val & 0xff) + carry) > 0xff), (pz80->registers[REGA] + val + carry) > 0xffff);
	pz80->registers[REGA] = pz80->registers[REGA] + val + carry;
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void ADC_A_immediate(z80*pz80)
{
	uint8_t carry = getFlag(pz80->registers[REGF], CARRY);
	uint8_t val = getImmediate(pz80);
	pz80->registers[REGF] = buildStatusFlag(((pz80->registers[REGA] + val + carry)==0), 0, (((pz80->registers[REGA] & 0xff) + (val & 0xff) + carry) > 0xff), (pz80->registers[REGA] + val + carry) > 0xffff);
	pz80->registers[REGA] = pz80->registers[REGA] + val + carry;
	pz80->tcycles = 8;
	incPC(pz80, 2);
}

void ADD_A_n(z80*pz80, int src)
{
	pz80->registers[REGF] = buildStatusFlag(((pz80->registers[REGA] + pz80->registers[src])==0), 0, (((pz80->registers[REGA] & 0xff) + (pz80->registers[src] & 0xff)) > 0xff), (pz80->registers[REGA] + pz80->registers[src]) > 0xffff);
	pz80->registers[REGA] = pz80->registers[REGA] + pz80->registers[src];
	pz80->tcycles = 4;
	incPC(pz80, 1);
}

void ADD_A_nn_mem(z80*pz80, int srcH, int srcL)
{
	uint8_t val = rb(&(pz80->mmu), (pz80->registers[srcH] << 8) + pz80->registers[srcL]);
	pz80->registers[REGF] = buildStatusFlag(((pz80->registers[REGA] + val)==0), 0, (((pz80->registers[REGA] & 0xff) + (val & 0xff)) > 0xff), (pz80->registers[REGA] + val) > 0xffff);
	pz80->registers[REGA] = pz80->registers[REGA] + val;
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void ADD_A_immediate(z80*pz80)
{
	uint8_t val = getImmediate(pz80);
	pz80->registers[REGF] = buildStatusFlag(((pz80->registers[REGA] + val)==0), 0, (((pz80->registers[REGA] & 0xff) + (val & 0xff)) > 0xff), (pz80->registers[REGA] + val) > 0xffff);
	pz80->registers[REGA] = pz80->registers[REGA] + val;
	pz80->tcycles = 8;
	incPC(pz80, 2);
}

void ADD_HL_nn(z80*pz80, int srcH, int srcL)
{
	uint8_t val = getRegister16(pz80, srcH, srcL);
	uint8_t hl_val = getRegister16(pz80, REGH, REGL);
	pz80->registers[REGF] = buildStatusFlag(((hl_val + val)==0), 0, (((hl_val & 0xffff) + (val & 0xffff)) > 0xffff), (hl_val + val) > 0xffff);
	setRegister16(pz80, REGH, REGL, (hl_val + val));
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void ADD_HL_SP(z80*pz80)
{
	uint8_t val = pz80->registers16[SP];
	uint8_t hl_val = getRegister16(pz80, REGH, REGL);
	pz80->registers[REGF] = buildStatusFlag(((hl_val + val)==0), 0, (((hl_val & 0xffff) + (val & 0xffff)) > 0xffff), (hl_val + val) > 0xffff);
	setRegister16(pz80, REGH, REGL, (hl_val + val));
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void ADD_SP_d(z80*pz80)
{
	int val = rb(&(pz80->mmu), pz80->registers16[PC]+1);
	if(val > 127)
		val = -((~val + 1) & 255);
	pz80->registers[REGF] = buildStatusFlag(0, 0, (((pz80->registers16[SP] & 0xff) + (val & 0xff)) > 0xff), (((int)pz80->registers16[SP]) + val) > 0xffff);
	pz80->registers16[SP] += val;
	pz80->tcycles = 16;
	incPC(pz80, 2);
}

void SUB_A_n(z80*pz80, int n)
{
	pz80->registers[REGF] = buildStatusFlag(((pz80->registers[REGA] - pz80->registers[n]) == 0), 1, ((pz80->registers[n] & 0xff) > (pz80->registers[REGA] & 0xff)), (pz80->registers[n] > pz80->registers[REGA]));
	pz80->registers[REGA] -= pz80->registers[n];
	pz80->tcycles = 4;
	incPC(pz80, 1);
}

void SUB_A_HL_mem(z80*pz80) 
{
	uint8_t val = rb(&(pz80->mmu), (pz80->registers[REGH] << 8) + pz80->registers[REGL]);
	pz80->registers[REGF] = buildStatusFlag(((pz80->registers[REGA] - val) == 0), 1, ((val & 0xff) > (pz80->registers[REGA] & 0xff)), (val > pz80->registers[REGA])); 
	pz80->registers[REGA] -= val;
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void SUB_A_immediate(z80*pz80)
{
	uint8_t val = getImmediate(pz80);
	pz80->registers[REGF] = buildStatusFlag(((pz80->registers[REGA] - val) == 0), 1, ((val & 0xff) > (pz80->registers[REGA] & 0xff)), (val > pz80->registers[REGA])); 
	pz80->registers[REGA] -= val;
	pz80->tcycles = 8;
	incPC(pz80, 2);
}

void SBC_A_n(z80*pz80, int n)
{
	uint8_t carry = getFlag(pz80->registers[REGF], CARRY);
	pz80->registers[REGF] = buildStatusFlag(((pz80->registers[REGA] - pz80->registers[n] - carry) == 0), 1, (((pz80->registers[n] & 0xff) + carry) > (pz80->registers[REGA] & 0xff)), ((pz80->registers[n] + carry) > pz80->registers[REGA]));
	pz80->registers[REGA] -= (pz80->registers[n] + carry);
	pz80->tcycles = 4;
	incPC(pz80, 1);
}

void SBC_A_HL_mem(z80*pz80) 
{
	uint8_t carry = getFlag(pz80->registers[REGF], CARRY);
	uint8_t val = rb(&(pz80->mmu), (pz80->registers[REGH] << 8) + pz80->registers[REGL]);
	pz80->registers[REGF] = buildStatusFlag(((pz80->registers[REGA] - val - carry) == 0), 1, (((val & 0xff) + carry) > (pz80->registers[REGA] & 0xff)), ((val + carry) > pz80->registers[REGA])); 
	pz80->registers[REGA] -= (val + carry);
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void SBC_A_immediate(z80*pz80)
{
	uint8_t carry = getFlag(pz80->registers[REGF], CARRY);
	uint8_t val = getImmediate(pz80);
	pz80->registers[REGF] = buildStatusFlag(((pz80->registers[REGA] - val - carry) == 0), 1, (((val & 0xff) + carry) > (pz80->registers[REGA] & 0xff)), ((val + carry) > pz80->registers[REGA])); 
	pz80->registers[REGA] -= (val + carry);
	pz80->tcycles = 8;
	incPC(pz80, 2);
}

void AND_A_n(z80*pz80, int n)
{
	pz80->registers[REGA] &= pz80->registers[n];
	pz80->registers[REGF] = buildStatusFlag((pz80->registers[REGA] == 0), 0, 1, 0);
	pz80->tcycles = 4;
	incPC(pz80, 1);
}

void AND_A_HL_mem(z80*pz80)
{
	uint8_t val = rb(&(pz80->mmu), (pz80->registers[REGH] << 8) + pz80->registers[REGL]);
	pz80->registers[REGA] &= val;
	pz80->registers[REGF] = buildStatusFlag((pz80->registers[REGA] == 0), 0, 1, 0);
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void AND_A_immediate(z80*pz80)
{
	uint8_t val = getImmediate(pz80);
	pz80->registers[REGA] &= val;
	pz80->registers[REGF] = buildStatusFlag((pz80->registers[REGA] == 0), 0, 1, 0);
	pz80->tcycles = 8;
	incPC(pz80, 2);
}

void XOR_A_n(z80*pz80, int n)
{
	pz80->registers[REGA] ^= pz80->registers[n];
	pz80->registers[REGF] = buildStatusFlag((pz80->registers[REGA] == 0), 0, 0, 0);
	pz80->tcycles = 4;
	incPC(pz80, 1);
}

void XOR_A_HL_mem(z80*pz80)
{
	uint8_t val = rb(&(pz80->mmu), (pz80->registers[REGH] << 8) + pz80->registers[REGL]);
	pz80->registers[REGA] ^= val;
	pz80->registers[REGF] = buildStatusFlag((pz80->registers[REGA] == 0), 0, 0, 0);
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void XOR_A_immediate(z80*pz80)
{
	uint8_t val = getImmediate(pz80);
	pz80->registers[REGA] ^= val;
	pz80->registers[REGF] = buildStatusFlag((pz80->registers[REGA] == 0), 0, 0, 0);
	pz80->tcycles = 8;
	incPC(pz80, 2);
}

void OR_A_n(z80*pz80, int n)
{
	pz80->registers[REGA] |= pz80->registers[n];
	pz80->registers[REGF] = buildStatusFlag((pz80->registers[REGA] == 0), 0, 0, 0);
	pz80->tcycles = 4;
	incPC(pz80, 1);
}

void OR_A_HL_mem(z80*pz80)
{
	uint8_t val = rb(&(pz80->mmu), (pz80->registers[REGH] << 8) + pz80->registers[REGL]);
	pz80->registers[REGA] |= val;
	pz80->registers[REGF] = buildStatusFlag((pz80->registers[REGA] == 0), 0, 0, 0);
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void OR_A_immediate(z80*pz80)
{
	uint8_t val = getImmediate(pz80);
	pz80->registers[REGA] |= val;
	pz80->registers[REGF] = buildStatusFlag((pz80->registers[REGA] == 0), 0, 0, 0);
	pz80->tcycles = 8;
	incPC(pz80, 2);
}

void CP_A_n(z80*pz80, int n)
{
	pz80->registers[REGF] = buildStatusFlag(((pz80->registers[REGA] - pz80->registers[n]) == 0), 1, ((pz80->registers[n] & 0xff) > (pz80->registers[REGA] & 0xff)), (pz80->registers[n] > pz80->registers[REGA]));
	pz80->tcycles = 4;
	incPC(pz80, 1);
}

void CP_A_HL_mem(z80*pz80) 
{
	uint8_t val = rb(&(pz80->mmu), (pz80->registers[REGH] << 8) + pz80->registers[REGL]);
	pz80->registers[REGF] = buildStatusFlag(((pz80->registers[REGA] - val) == 0), 1, ((val & 0xff) > (pz80->registers[REGA] & 0xff)), (val > pz80->registers[REGA])); 
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void CP_A_immediate(z80*pz80)
{
	uint8_t val = getImmediate(pz80);
	pz80->registers[REGF] = buildStatusFlag(((pz80->registers[REGA] - val) == 0), 1, ((val & 0xff) > (pz80->registers[REGA] & 0xff)), (val > pz80->registers[REGA])); 
	pz80->tcycles = 8;
	incPC(pz80, 2);
}

void LD_nn_immediate(z80*pz80, int regH, int regL)
{
	loadRegFromMemory16(pz80, pz80->registers16[PC]+1, regH, regL);
	pz80->tcycles = 12;
	incPC(pz80, 3);
}

void LD_SP_HL(z80*pz80)
{
	pz80->registers16[SP] = (pz80->registers[REGH] << 8) + pz80->registers[REGL];
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void LDHL_SP_immediate(z80*pz80)
{
	int val = getImmediate(pz80);
	if(val > 127)
	{
		val = -((~val + 1) & 255);
	}
	pz80->registers[REGF] = buildStatusFlag(0, 0, (((pz80->registers16[SP] & 0xff) + val) > 0xff), (((unsigned int)(pz80->registers16[SP])+(unsigned int)(val)) > 0xffff));
	setRegister16(pz80, REGH, REGL, (pz80->registers16[SP]+val));
	pz80->tcycles = 12;
	incPC(pz80, 2);
}

void LD_nn_SP(z80*pz80)
{
	uint16_t addr = getImmediate16(pz80);
	ww(&(pz80->mmu), addr, pz80->registers16[SP]);
	pz80->tcycles = 20;
	incPC(pz80, 3);
}

void LD_n_n(z80*pz80, int reg1, int reg2)
{
	pz80->registers[reg1] = pz80->registers[reg2];
	pz80->tcycles = 4;
	incPC(pz80, 1);
}

void LD_n_nn_mem(z80*pz80, int reg1, int nH, int nL)
{
	pz80->registers[reg1] = rb(&(pz80->mmu), ((uint16_t)(pz80->registers[nH]) << 8) + (uint16_t)pz80->registers[nL]);
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void LD_nn_mem_n(z80*pz80, int nH, int nL, int reg2)
{
	wb(&(pz80->mmu), ((uint16_t)(pz80->registers[nH]) << 8) + (uint16_t)pz80->registers[nL], pz80->registers[reg2]); 
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void LD_nn_mem_immediate(z80*pz80, int nH, int nL)
{
	uint8_t val = getImmediate(pz80);
	wb(&(pz80->mmu), ((uint16_t)(pz80->registers[nH]) << 8) + (uint16_t)pz80->registers[nL], val); 
	pz80->tcycles = 12;
	incPC(pz80, 2);
}

void LD_n_immediate_mem(z80*pz80, int n)
{
	pz80->registers[n] = rb(&(pz80->mmu),getImmediate16(pz80));
	pz80->tcycles = 16;
	incPC(pz80, 3);	
}

void LD_n_immediate(z80*pz80, int n)
{
	pz80->registers[n] = getImmediate(pz80);
	pz80->tcycles = 8;
	incPC(pz80, 2);		
	
	#ifdef DEBUG
		if(n == REGB)
		{
			printf("LD_B_immediate, immediate: %d ", pz80->registers[n]);
		}
	#endif
}

void LD_immediate_mem_n(z80*pz80, int n)
{
	wb(&(pz80->mmu), getImmediate16(pz80), pz80->registers[n]);
	pz80->tcycles = 16;
	incPC(pz80, 3);
}

void LDH_immediate_A(z80*pz80)
{
	wb(&(pz80->mmu), 0xff00 + getImmediate(pz80), pz80->registers[REGA]);
	pz80->tcycles = 12;
	incPC(pz80, 2);
}

void LDH_A_immediate(z80*pz80)
{
	pz80->registers[REGA] = rb(&(pz80->mmu), 0xff00 + getImmediate(pz80));
	pz80->tcycles = 12;
	incPC(pz80, 2);
}

void LDD_A_HL_mem(z80*pz80)
{
	LD_n_nn_mem(pz80, REGA, REGH, REGL);
	DEC_nn(pz80, REGH, REGL);
	pz80->tcycles = 8;
	incPC(pz80, -1); // as DEC_nn and LD_nn_mem_n increase the pc by 1
}

void LDD_HL_mem_A(z80*pz80)
{
	LD_nn_mem_n(pz80, REGH, REGL, REGA);
	DEC_nn(pz80, REGH, REGL);
	pz80->tcycles = 8;
	incPC(pz80, -1); // as DEC_nn and LD_nn_mem_n increase the pc by 1
}

void LDI_A_HL_mem(z80*pz80)
{
	LD_n_nn_mem(pz80, REGA, REGH, REGL);
	INC_nn(pz80, REGH, REGL);
	pz80->tcycles = 8;
	incPC(pz80, -1); // as DEC_nn and LD_nn_mem_n increase the pc by 1
}

void LDI_HL_mem_A(z80*pz80)
{
	LD_nn_mem_n(pz80, REGH, REGL, REGA);
	INC_nn(pz80, REGH, REGL);
	pz80->tcycles = 8;
	incPC(pz80, -1); // as DEC_nn and LD_nn_mem_n increase the pc by 1
}

void LD_C_mem_A(z80*pz80)
{
	wb(&(pz80->mmu), (0xff00 + pz80->registers[REGC]),pz80->registers[REGA]);
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void LD_SP_nn(z80*pz80)
{
	pz80->registers16[SP] = getImmediate16(pz80);
	pz80->tcycles = 12;
	incPC(pz80, 3);
}

void JP_nn(z80*pz80)
{
	pz80->registers16[PC] = getImmediate16(pz80);	
	pz80->tcycles = 12;
}

void JP_NZ_nn(z80*pz80)
{
	if(!getFlag(pz80->registers[REGF], ZERO))
	{
		JP_nn(pz80);
	}
	else
	{
		pz80->tcycles = 12;
		incPC(pz80, 3);
	}
}

void JP_Z_nn(z80*pz80)
{
	if(getFlag(pz80->registers[REGF], ZERO))
	{
		JP_nn(pz80);
	}
	else
	{
		pz80->tcycles = 12;
		incPC(pz80, 3);
	}	
}

void JP_NC_nn(z80*pz80)
{
	if(!getFlag(pz80->registers[REGF], CARRY))
	{
		JP_nn(pz80);
	}
	else
	{
		pz80->tcycles = 12;
		incPC(pz80, 3);
	}
}

void JP_C_nn(z80*pz80)
{
	if(getFlag(pz80->registers[REGF], CARRY))
	{
		JP_nn(pz80);
	}
	else
	{
		pz80->tcycles = 12;
		incPC(pz80, 3);
	}	
}

void JP_HL(z80*pz80)
{
	pz80->registers16[PC] = (((uint16_t)pz80->registers[REGH]) << 8) + (uint16_t)pz80->registers[REGL];
	pz80->tcycles = 4;
	incPC(pz80, 1);
}

void JR_n(z80*pz80)
{
	int rjump = getImmediate(pz80);
	if(rjump > 127)
		rjump = -((~rjump + 1) & 255);
	pz80->registers16[PC] += rjump;
	pz80->tcycles = 8;
	incPC(pz80, 2);
}

void JR_NZ_n(z80*pz80)
{
	if(!getFlag(pz80->registers[REGF], ZERO))
	{
		JR_n(pz80);
	}
	else
	{
		pz80->tcycles = 8;
		incPC(pz80, 2);
	}
}

void JR_Z_n(z80*pz80)
{
	if(getFlag(pz80->registers[REGF], ZERO))
	{
		JR_n(pz80);
	}
	else
	{
		pz80->tcycles = 8;
		incPC(pz80, 2);
	}
}

void JR_NC_n(z80*pz80)
{
	if(!getFlag(pz80->registers[REGF], CARRY))
	{
		JR_n(pz80);
	}
	else
	{
		pz80->tcycles = 8;
		incPC(pz80, 2);
	}
}

void JR_C_n(z80*pz80)
{
	if(getFlag(pz80->registers[REGF], CARRY))
	{
		JR_n(pz80);
	}
	else
	{
		pz80->tcycles = 8;
		incPC(pz80, 2);
	}
}

void push(z80*pz80, uint8_t val)
{
	pz80->registers16[SP]--;
	wb(&(pz80->mmu), pz80->registers16[SP], val);
	//printf("Push: writing: %d at addr: %x\n", val, pz80->registers16[SP]);
}

uint8_t pop(z80*pz80)
{
	pz80->registers16[SP]++;
	uint8_t val = rb(&(pz80->mmu), pz80->registers16[SP] - 1);
	//printf("Pop: read %d from %x\n", val, pz80->registers16[SP] - 1);
	return val;
}

void CALL_nn(z80*pz80)
{
	push(pz80, (pz80->registers16[PC]+3) >> 8);
	push(pz80, (pz80->registers16[PC]+3) & 255);
	pz80->registers16[PC] = getImmediate16(pz80);
	pz80->tcycles = 12;
	#ifdef DEBUG
		printf("CALL_nn, address: %x\n", pz80->registers16[PC]);
	#endif
}

void CALL_NZ_nn(z80*pz80)
{
	if(!getFlag(pz80->registers[REGF], ZERO))
	{
		CALL_nn(pz80);
	}
	else
	{
		pz80->tcycles = 12;
		incPC(pz80, 3);
	}
}

void CALL_Z_nn(z80*pz80)
{
	if(getFlag(pz80->registers[REGF], ZERO))
	{
		CALL_nn(pz80);
	}
	else
	{
		pz80->tcycles = 12;
		incPC(pz80, 3);
	}
}

void CALL_NC_nn(z80*pz80)
{
	if(!getFlag(pz80->registers[REGF], CARRY))
	{
		CALL_nn(pz80);
	}
	else
	{
		pz80->tcycles = 12;
		incPC(pz80, 3);
	}
}

void CALL_C_nn(z80*pz80)
{
	if(getFlag(pz80->registers[REGF], CARRY))
	{
		CALL_nn(pz80);
	}
	else
	{
		pz80->tcycles = 12;
		incPC(pz80, 3);
	}
}

void RST_n(z80*pz80, int n)
{
	push(pz80, pz80->registers16[PC] >> 8);
	push(pz80, pz80->registers16[PC] & 255);
	pz80->registers16[PC] = n;
	pz80->tcycles = 32;
}

void RET(z80*pz80)
{
	pz80->registers16[PC] = (uint16_t)pop(pz80) + (((uint16_t)pop(pz80)) << 8) ;
	pz80->tcycles = 8;
}

void RET_NZ(z80*pz80)
{
	if(!getFlag(pz80->registers[REGF], ZERO))
	{
		RET(pz80);
	}
	else
	{
		pz80->tcycles = 8; // ?
		incPC(pz80, 1);
	}
}

void RET_Z(z80*pz80)
{
	if(getFlag(pz80->registers[REGF], ZERO))
	{
		RET(pz80);
	}
	else
	{
		pz80->tcycles = 8; // ?
		incPC(pz80, 1);
	}
}

void RET_NC(z80*pz80)
{
	if(!getFlag(pz80->registers[REGF], CARRY))
	{
		RET(pz80);
	}
	else
	{
		pz80->tcycles = 8; // ?
		incPC(pz80, 1);
	}
}

void RET_C(z80*pz80)
{
	if(getFlag(pz80->registers[REGF], CARRY))
	{
		RET(pz80);
	}
	else
	{
		pz80->tcycles = 8; // ?
		incPC(pz80, 1);
	}
}

void RETI(z80*pz80)
{
	RET(pz80);
	pz80->ime = 1;
}

void PUSH_nn(z80*pz80, uint8_t nH, uint8_t nL)
{
	push(pz80, pz80->registers[nH]);
	push(pz80, pz80->registers[nL]);
	pz80->tcycles = 16;
	incPC(pz80, 1);
}

void POP_nn(z80*pz80, int nH, int nL)
{
	pz80->registers[nL] = pop(pz80);
	pz80->registers[nH] = pop(pz80);
	pz80->tcycles = 12;
	incPC(pz80, 1);
}

void
SRL_n(z80*pz80, int reg){
	uint8_t newFlag = 0;
	uint8_t byte = pz80->registers[reg];
	uint8_t newbyte = byte >> 1;
	if(byte&1)
		setFlag(newFlag,CARRY);
	if(newbyte==0)
		setFlag(newFlag,ZERO);
	pz80->registers[REGF] = newFlag;
	pz80->registers[reg] = newbyte;
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void
SRL_HL(z80*pz80){
	loadRegMemFromHL(pz80);
	SRL_n(pz80,REGMEM);
	saveRegMemToHL(pz80);
	pz80->tcycles = 16;
}

void
SRA_n(z80*pz80, int reg){
	uint8_t newFlag = 0;
	uint8_t byte = pz80->registers[reg];
	uint8_t newbyte = byte >> 1;
	if(byte & (1<<7)) // MSB doesnt change
		newbyte |= (1<<7);
	if(byte&1)
		setFlag(newFlag,CARRY);
	if(newbyte==0)
		setFlag(newFlag,ZERO);
	pz80->registers[REGF] = newFlag;
	pz80->registers[reg] = newbyte;
	pz80->tcycles = 8;
	incPC(pz80, 1);
}



void
SRA_HL(z80*pz80){
	loadRegMemFromHL(pz80);
	SRA_n(pz80,REGMEM);
	saveRegMemToHL(pz80);
	pz80->tcycles = 16;
	//DO not incPC as sub function does this
}


void
SLA_n(z80*pz80, int reg){
	uint8_t newFlag = 0;
	uint8_t byte = pz80->registers[reg];
	uint8_t newbyte = byte << 1;
	if(byte&(1<<7))
		setFlag(newFlag,CARRY);
	if(newbyte==0)
		setFlag(newFlag,ZERO);
	pz80->registers[REGF] = newFlag;
	pz80->registers[reg] = newbyte;
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void
SLA_HL(z80*pz80){
	loadRegMemFromHL(pz80);
	SLA_n(pz80,REGMEM);
	saveRegMemToHL(pz80);
	pz80->tcycles = 16;
	//DO not incPC as sub function does this
}


void RR_n(z80*pz80,int reg){
	uint8_t newFlag = 0;
	uint8_t byte = pz80->registers[reg];
	uint8_t newbyte =  (byte>>1);
	if(getFlag(pz80->registers[REGF],CARRY))
		newbyte+=(1<< 7);
	if(byte&1)
		setFlag(newFlag,CARRY);
	if(newbyte==0)
		setFlag(newFlag,ZERO);
	pz80->registers[REGF] = newFlag;
	pz80->registers[reg] = newbyte;
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void RR_HL(z80*pz80){
	loadRegMemFromHL(pz80);
	RR_n(pz80,REGMEM);
	saveRegMemToHL(pz80);
	pz80->tcycles = 16;
	//DO not incPC as sub function does this
}

void RRC_n(z80*pz80,int reg){
	uint8_t newFlag = 0;
	uint8_t byte = pz80->registers[reg];
	uint8_t newbyte =  (byte>>1);
	if(byte&1){
		setFlag(newFlag,CARRY);
		newbyte+=  1<<7 ;
	}
	if(newbyte==0)
		setFlag(newFlag,ZERO);
	pz80->registers[REGF] = newFlag;
	pz80->registers[reg] = newbyte;
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void RRC_HL(z80*pz80){
	loadRegMemFromHL(pz80);
	RRC_n(pz80,REGMEM);
	saveRegMemToHL(pz80);
	pz80->tcycles = 16;
	//DO not incPC as sub function does this
}

void RL_n(z80*pz80,int reg){
	uint8_t newFlag = 0;
	uint8_t byte = pz80->registers[reg];
	uint8_t newbyte =  (byte<<1);
	if(getFlag(pz80->registers[REGF],CARRY))
		newbyte+=1;
	if(byte&(1<<7)) // if MSB
		setFlag(newFlag,CARRY);
	if(newbyte==0)
		setFlag(newFlag,ZERO);
	pz80->registers[REGF] = newFlag;
	pz80->registers[reg] = newbyte;
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void RL_HL(z80*pz80){
	loadRegMemFromHL(pz80);
	RL_n(pz80,REGMEM);
	saveRegMemToHL(pz80);
	pz80->tcycles = 16;
	//DO not incPC as sub function does this
}


void RLC_n(z80*pz80,int reg){
	uint8_t newFlag = 0;
	uint8_t byte = pz80->registers[reg];
	uint8_t newbyte =  (byte<<1);
	if(byte&(1<<7)){
		newbyte+= 1;
		setFlag(newFlag,CARRY);
	}
	if(newbyte==0)
		setFlag(newFlag,ZERO);
	pz80->registers[REGF] = newFlag;
	pz80->registers[reg] = newbyte;
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void RLC_HL(z80*pz80){
	loadRegMemFromHL(pz80);
	RLC_n(pz80,REGMEM);
	saveRegMemToHL(pz80);
	pz80->tcycles = 16;
	//DO not incPC as sub function does this
}


void Test_Bit(z80*pz80, int reg,int bit){
	pz80->registers[REGF] = buildStatusFlag( !((pz80->registers[reg]) & (1 << bit)) ,
												0,
												1,
												getFlag(pz80->registers[REGF],CARRY) );
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void Test_HLBit(z80*pz80,int bit){ 
	loadRegMemFromHL(pz80);
	Test_Bit(pz80,REGMEM,bit);
	saveRegMemToHL(pz80);
	pz80->tcycles = 16;
	//DO not incPC as sub function does this
}

/* these two nibble swap functions dont use build status flag because most are fixed and this is much faster */
void nibbleSwap(z80*pz80,int reg){
	uint8_t byte = pz80->registers[reg];
	if(byte == 0){// no need to swap it if its zero
		pz80->registers[REGF] = (1<<ZERO);
		return;
	}
	 pz80->registers[REGF] = 0;
	 pz80->registers[reg] = ( (byte >> 4) & 0x0f/*0b00001111*/) | ( (byte << 4) & 0xf0/*0b11110000*/ );
	pz80->tcycles = 8;
	incPC(pz80, 1);
}

void nibbleHLSwap(z80*pz80){
	loadRegMemFromHL(pz80);
	nibbleSwap(pz80,REGMEM);
	saveRegMemToHL(pz80);
	pz80->tcycles = 16;
	//DO not incPC as sub function does this
}



/* set and reset the same */
void Set_BitToVal(z80*pz80, int reg,int bit,int val){
	if(val)
		pz80->registers[reg] |= (1<<bit);
	else
		pz80->registers[reg] &= ~(1<<bit);
	pz80->tcycles = 8;
	incPC(pz80, 1);
}


/* set and reset the same */
void Set_HLBitToVal(z80*pz80,int bit,int val){
	loadRegMemFromHL(pz80);
	Set_BitToVal(pz80,REGMEM,bit,val);
	saveRegMemToHL(pz80);
	pz80->tcycles = 16;
	//DO not incPC as sub function does this
}


/* tcycles and pc are updated by instructions */
void executeNextInstruction(z80 * pz80){
	MMU * pmmu = &(pz80->mmu);
	uint16_t insAddress =  pz80->registers16[PC];
	uint8_t	instruction = rb(pmmu,insAddress);
	#ifdef DEBUG
		printf("PC: %d, Opcode: 0x%x\n", insAddress, instruction);
	#endif
	dispatchInstruction(pz80,instruction,0/*pz80->doSecondaryOpcode*/);
	//printf("Instruction dispatched\n");
	gpu_step(&(pz80->mmu.gpu), pz80->tcycles);
}



void 
dispatchInstruction(z80 * pz80,uint8_t opcode, int secondary){

	if(!secondary){
		switch(opcode){
			case 0x00:
				i_NOP(pz80);
				return;
			case 0x01:
				i_LD_BC_nn(pz80);
				return;
			case 0x02:
				i_LD__BC__A(pz80);
				return;
			case 0x03:
				i_INC_BC(pz80);
				return;
			case 0x04:
				i_INC_B(pz80);
				return;
			case 0x05:
				i_DEC_B(pz80);
				return;
			case 0x06:
				i_LD_B_n(pz80);
				return;
			case 0x07:
				i_RLC_A(pz80);
				return;
			case 0x08:
				i_LD__nn__SP(pz80);
				return;
			case 0x09:
				i_ADD_HL_BC(pz80);
				return;
			case 0x0A:
				i_LD_A__BC_(pz80);
				return;
			case 0x0B:
				i_DEC_BC(pz80);
				return;
			case 0x0C:
				i_INC_C(pz80);
				return;
			case 0x0D:
				i_DEC_C(pz80);
				return;
			case 0x0E:
				i_LD_C_n(pz80);
				return;
			case 0x0F:
				i_RRC_A(pz80);
				return;
			case 0x10:
				i_STOP(pz80);
				return;
			case 0x11:
				i_LD_DE_nn(pz80);
				return;
			case 0x12:
				i_LD__DE__A(pz80);
				return;
			case 0x13:
				i_INC_DE(pz80);
				return;
			case 0x14:
				i_INC_D(pz80);
				return;
			case 0x15:
				i_DEC_D(pz80);
				return;
			case 0x16:
				i_LD_D_n(pz80);
				return;
			case 0x17:
				i_RL_A(pz80);
				return;
			case 0x18:
				i_JR_n(pz80);
				return;
			case 0x19:
				i_ADD_HL_DE(pz80);
				return;
			case 0x1A:
				i_LD_A__DE_(pz80);
				return;
			case 0x1B:
				i_DEC_DE(pz80);
				return;
			case 0x1C:
				i_INC_E(pz80);
				return;
			case 0x1D:
				i_DEC_E(pz80);
				return;
			case 0x1E:
				i_LD_E_n(pz80);
				return;
			case 0x1F:
				i_RR_A(pz80);
				return;
			case 0x20:
				i_JR_NZ_n(pz80);
				return;
			case 0x21:
				i_LD_HL_nn(pz80);
				return;
			case 0x22:
				i_LDI__HL__A(pz80);
				return;
			case 0x23:
				i_INC_HL(pz80);
				return;
			case 0x24:
				i_INC_H(pz80);
				return;
			case 0x25:
				i_DEC_H(pz80);
				return;
			case 0x26:
				i_LD_H_n(pz80);
				return;
			case 0x27:
				i_DAA(pz80);
				return;
			case 0x28:
				i_JR_Z_n(pz80);
				return;
			case 0x29:
				i_ADD_HL_HL(pz80);
				return;
			case 0x2A:
				i_LDI_A__HL_(pz80);
				return;
			case 0x2B:
				i_DEC_HL(pz80);
				return;
			case 0x2C:
				i_INC_L(pz80);
				return;
			case 0x2D:
				i_DEC_L(pz80);
				return;
			case 0x2E:
				i_LD_L_n(pz80);
				return;
			case 0x2F:
				i_CPL(pz80);
				return;
			case 0x30:
				i_JR_NC_n(pz80);
				return;
			case 0x31:
				i_LD_SP_nn(pz80);
				return;
			case 0x32:
				i_LDD__HL__A(pz80);
				return;
			case 0x33:
				i_INC_SP(pz80);
				return;
			case 0x34:
				i_INC__HL_(pz80);
				return;
			case 0x35:
				i_DEC__HL_(pz80);
				return;
			case 0x36:
				i_LD__HL__n(pz80);
				return;
			case 0x37:
				i_SCF(pz80);
				return;
			case 0x38:
				i_JR_C_n(pz80);
				return;
			case 0x39:
				i_ADD_HL_SP(pz80);
				return;
			case 0x3A:
				i_LDD_A__HL_(pz80);
				return;
			case 0x3B:
				i_DEC_SP(pz80);
				return;
			case 0x3C:
				i_INC_A(pz80);
				return;
			case 0x3D:
				i_DEC_A(pz80);
				return;
			case 0x3E:
				i_LD_A_n(pz80);
				return;
			case 0x3F:
				i_CCF(pz80);
				return;
			case 0x40:
				i_LD_B_B(pz80);
				return;
			case 0x41:
				i_LD_B_C(pz80);
				return;
			case 0x42:
				i_LD_B_D(pz80);
				return;
			case 0x43:
				i_LD_B_E(pz80);
				return;
			case 0x44:
				i_LD_B_H(pz80);
				return;
			case 0x45:
				i_LD_B_L(pz80);
				return;
			case 0x46:
				i_LD_B__HL_(pz80);
				return;
			case 0x47:
				i_LD_B_A(pz80);
				return;
			case 0x48:
				i_LD_C_B(pz80);
				return;
			case 0x49:
				i_LD_C_C(pz80);
				return;
			case 0x4A:
				i_LD_C_D(pz80);
				return;
			case 0x4B:
				i_LD_C_E(pz80);
				return;
			case 0x4C:
				i_LD_C_H(pz80);
				return;
			case 0x4D:
				i_LD_C_L(pz80);
				return;
			case 0x4E:
				i_LD_C__HL_(pz80);
				return;
			case 0x4F:
				i_LD_C_A(pz80);
				return;
			case 0x50:
				i_LD_D_B(pz80);
				return;
			case 0x51:
				i_LD_D_C(pz80);
				return;
			case 0x52:
				i_LD_D_D(pz80);
				return;
			case 0x53:
				i_LD_D_E(pz80);
				return;
			case 0x54:
				i_LD_D_H(pz80);
				return;
			case 0x55:
				i_LD_D_L(pz80);
				return;
			case 0x56:
				i_LD_D__HL_(pz80);
				return;
			case 0x57:
				i_LD_D_A(pz80);
				return;
			case 0x58:
				i_LD_E_B(pz80);
				return;
			case 0x59:
				i_LD_E_C(pz80);
				return;
			case 0x5A:
				i_LD_E_D(pz80);
				return;
			case 0x5B:
				i_LD_E_E(pz80);
				return;
			case 0x5C:
				i_LD_E_H(pz80);
				return;
			case 0x5D:
				i_LD_E_L(pz80);
				return;
			case 0x5E:
				i_LD_E__HL_(pz80);
				return;
			case 0x5F:
				i_LD_E_A(pz80);
				return;
			case 0x60:
				i_LD_H_B(pz80);
				return;
			case 0x61:
				i_LD_H_C(pz80);
				return;
			case 0x62:
				i_LD_H_D(pz80);
				return;
			case 0x63:
				i_LD_H_E(pz80);
				return;
			case 0x64:
				i_LD_H_H(pz80);
				return;
			case 0x65:
				i_LD_H_L(pz80);
				return;
			case 0x66:
				i_LD_H__HL_(pz80);
				return;
			case 0x67:
				i_LD_H_A(pz80);
				return;
			case 0x68:
				i_LD_L_B(pz80);
				return;
			case 0x69:
				i_LD_L_C(pz80);
				return;
			case 0x6A:
				i_LD_L_D(pz80);
				return;
			case 0x6B:
				i_LD_L_E(pz80);
				return;
			case 0x6C:
				i_LD_L_H(pz80);
				return;
			case 0x6D:
				i_LD_L_L(pz80);
				return;
			case 0x6E:
				i_LD_L__HL_(pz80);
				return;
			case 0x6F:
				i_LD_L_A(pz80);
				return;
			case 0x70:
				i_LD__HL__B(pz80);
				return;
			case 0x71:
				i_LD__HL__C(pz80);
				return;
			case 0x72:
				i_LD__HL__D(pz80);
				return;
			case 0x73:
				i_LD__HL__E(pz80);
				return;
			case 0x74:
				i_LD__HL__H(pz80);
				return;
			case 0x75:
				i_LD__HL__L(pz80);
				return;
			case 0x76:
				i_HALT(pz80);
				return;
			case 0x77:
				i_LD__HL__A(pz80);
				return;
			case 0x78:
				i_LD_A_B(pz80);
				return;
			case 0x79:
				i_LD_A_C(pz80);
				return;
			case 0x7A:
				i_LD_A_D(pz80);
				return;
			case 0x7B:
				i_LD_A_E(pz80);
				return;
			case 0x7C:
				i_LD_A_H(pz80);
				return;
			case 0x7D:
				i_LD_A_L(pz80);
				return;
			case 0x7E:
				i_LD_A__HL_(pz80);
				return;
			case 0x7F:
				i_LD_A_A(pz80);
				return;
			case 0x80:
				i_ADD_A_B(pz80);
				return;
			case 0x81:
				i_ADD_A_C(pz80);
				return;
			case 0x82:
				i_ADD_A_D(pz80);
				return;
			case 0x83:
				i_ADD_A_E(pz80);
				return;
			case 0x84:
				i_ADD_A_H(pz80);
				return;
			case 0x85:
				i_ADD_A_L(pz80);
				return;
			case 0x86:
				i_ADD_A__HL_(pz80);
				return;
			case 0x87:
				i_ADD_A_A(pz80);
				return;
			case 0x88:
				i_ADC_A_B(pz80);
				return;
			case 0x89:
				i_ADC_A_C(pz80);
				return;
			case 0x8A:
				i_ADC_A_D(pz80);
				return;
			case 0x8B:
				i_ADC_A_E(pz80);
				return;
			case 0x8C:
				i_ADC_A_H(pz80);
				return;
			case 0x8D:
				i_ADC_A_L(pz80);
				return;
			case 0x8E:
				i_ADC_A__HL_(pz80);
				return;
			case 0x8F:
				i_ADC_A_A(pz80);
				return;
			case 0x90:
				i_SUB_A_B(pz80);
				return;
			case 0x91:
				i_SUB_A_C(pz80);
				return;
			case 0x92:
				i_SUB_A_D(pz80);
				return;
			case 0x93:
				i_SUB_A_E(pz80);
				return;
			case 0x94:
				i_SUB_A_H(pz80);
				return;
			case 0x95:
				i_SUB_A_L(pz80);
				return;
			case 0x96:
				i_SUB_A__HL_(pz80);
				return;
			case 0x97:
				i_SUB_A_A(pz80);
				return;
			case 0x98:
				i_SBC_A_B(pz80);
				return;
			case 0x99:
				i_SBC_A_C(pz80);
				return;
			case 0x9A:
				i_SBC_A_D(pz80);
				return;
			case 0x9B:
				i_SBC_A_E(pz80);
				return;
			case 0x9C:
				i_SBC_A_H(pz80);
				return;
			case 0x9D:
				i_SBC_A_L(pz80);
				return;
			case 0x9E:
				i_SBC_A__HL_(pz80);
				return;
			case 0x9F:
				i_SBC_A_A(pz80);
				return;
			case 0xA0:
				i_AND_B(pz80);
				return;
			case 0xA1:
				i_AND_C(pz80);
				return;
			case 0xA2:
				i_AND_D(pz80);
				return;
			case 0xA3:
				i_AND_E(pz80);
				return;
			case 0xA4:
				i_AND_H(pz80);
				return;
			case 0xA5:
				i_AND_L(pz80);
				return;
			case 0xA6:
				i_AND__HL_(pz80);
				return;
			case 0xA7:
				i_AND_A(pz80);
				return;
			case 0xA8:
				i_XOR_B(pz80);
				return;
			case 0xA9:
				i_XOR_C(pz80);
				return;
			case 0xAA:
				i_XOR_D(pz80);
				return;
			case 0xAB:
				i_XOR_E(pz80);
				return;
			case 0xAC:
				i_XOR_H(pz80);
				return;
			case 0xAD:
				i_XOR_L(pz80);
				return;
			case 0xAE:
				i_XOR__HL_(pz80);
				return;
			case 0xAF:
				i_XOR_A(pz80);
				return;
			case 0xB0:
				i_OR_B(pz80);
				return;
			case 0xB1:
				i_OR_C(pz80);
				return;
			case 0xB2:
				i_OR_D(pz80);
				return;
			case 0xB3:
				i_OR_E(pz80);
				return;
			case 0xB4:
				i_OR_H(pz80);
				return;
			case 0xB5:
				i_OR_L(pz80);
				return;
			case 0xB6:
				i_OR__HL_(pz80);
				return;
			case 0xB7:
				i_OR_A(pz80);
				return;
			case 0xB8:
				i_CP_B(pz80);
				return;
			case 0xB9:
				i_CP_C(pz80);
				return;
			case 0xBA:
				i_CP_D(pz80);
				return;
			case 0xBB:
				i_CP_E(pz80);
				return;
			case 0xBC:
				i_CP_H(pz80);
				return;
			case 0xBD:
				i_CP_L(pz80);
				return;
			case 0xBE:
				i_CP__HL_(pz80);
				return;
			case 0xBF:
				i_CP_A(pz80);
				return;
			case 0xC0:
				i_RET_NZ(pz80);
				return;
			case 0xC1:
				i_POP_BC(pz80);
				return;
			case 0xC2:
				i_JP_NZ_nn(pz80);
				return;
			case 0xC3:
				i_JP_nn(pz80);
				return;
			case 0xC4:
				i_CALL_NZ_nn(pz80);
				return;
			case 0xC5:
				i_PUSH_BC(pz80);
				return;
			case 0xC6:
				i_ADD_A_n(pz80);
				return;
			case 0xC7:
				i_RST_0(pz80);
				return;
			case 0xC8:
				i_RET_Z(pz80);
				return;
			case 0xC9:
				i_RET(pz80);
				return;
			case 0xCA:
				i_JP_Z_nn(pz80);
				return;
			case 0xCB:
				i_Ext_ops(pz80);
				return;
			case 0xCC:
				i_CALL_Z_nn(pz80);
				return;
			case 0xCD:
				i_CALL_nn(pz80);
				return;
			case 0xCE:
				i_ADC_A_n(pz80);
				return;
			case 0xCF:
				i_RST_8(pz80);
				return;
			case 0xD0:
				i_RET_NC(pz80);
				return;
			case 0xD1:
				i_POP_DE(pz80);
				return;
			case 0xD2:
				i_JP_NC_nn(pz80);
				return;
			case 0xD3:
				//i_XX(pz80); not implemented
				return;
			case 0xD4:
				i_CALL_NC_nn(pz80);
				return;
			case 0xD5:
				i_PUSH_DE(pz80);
				return;
			case 0xD6:
				i_SUB_A_n(pz80);
				return;
			case 0xD7:
				i_RST_10(pz80);
				return;
			case 0xD8:
				i_RET_C(pz80);
				return;
			case 0xD9:
				i_RETI(pz80);
				return;
			case 0xDA:
				i_JP_C_nn(pz80);
				return;
			case 0xDB:
				//i_XX(pz80); not implemented
				return;
			case 0xDC:
				i_CALL_C_nn(pz80);
				return;
			case 0xDD:
				//i_XX(pz80); not implemented
				return;
			case 0xDE:
				i_SBC_A_n(pz80);
				return;
			case 0xDF:
				i_RST_18(pz80);
				return;
			case 0xE0:
				i_LDH__n__A(pz80);
				return;
			case 0xE1:
				i_POP_HL(pz80);
				return;
			case 0xE2:
				i_LDH__C__A(pz80);
				return;
			case 0xE3:
				i_NOP(pz80);
				//i_XX(pz80); not implemented
				return;
			case 0xE4:
				i_NOP(pz80);
				//i_XX(pz80); not implemented
				return;
			case 0xE5:
				i_PUSH_HL(pz80);
				return;
			case 0xE6:
				i_AND_n(pz80);
				return;
			case 0xE7:
				i_RST_20(pz80);
				return;
			case 0xE8:
				i_ADD_SP_d(pz80);
				return;
			case 0xE9:
				i_JP__HL_(pz80);
				return;
			case 0xEA:
				i_LD__nn__A(pz80);
				return;
			case 0xEB:
				i_NOP(pz80);
				//i_XX(pz80); not implemented
				return;
			case 0xEC:
				i_NOP(pz80);
				//i_XX(pz80); not implemented
				return;
			case 0xED:
				i_NOP(pz80);
				//i_XX(pz80); not implemented
				return;
			case 0xEE:
				i_XOR_n(pz80);
				return;
			case 0xEF:
				i_RST_28(pz80);
				return;
			case 0xF0:
				i_LDH_A__n_(pz80);
				return;
			case 0xF1:
				i_POP_AF(pz80);
				return;
			case 0xF2:
				i_NOP(pz80);
				//i_XX(pz80); not implemented
				return;
			case 0xF3:
				i_DI(pz80);
				return;
			case 0xF4:
				i_NOP(pz80);
				//i_XX(pz80); not implemented
				return;
			case 0xF5:
				i_PUSH_AF(pz80);
				return;
			case 0xF6:
				i_OR_n(pz80);
				return;
			case 0xF7:
				i_RST_30(pz80);
				return;
			case 0xF8:
				i_LDHL_SP_d(pz80);
				return;
			case 0xF9:
				i_LD_SP_HL(pz80);
				return;
			case 0xFA:
				i_LD_A__nn_(pz80);
				return;
			case 0xFB:
				i_EI(pz80);
				return;
			case 0xFC:
				i_NOP(pz80);
				//i_XX(pz80); not implemented
				return;
			case 0xFD:
				i_NOP(pz80);
				//i_XX(pz80); not implemented
				return;
			case 0xFE:
				i_CP_n(pz80);
				return;
			case 0xFF:
				i_RST_38(pz80);
				return;
		}
	}

	/********************/

	switch(opcode){
		case 0x00:
			i2_RLC_B(pz80);
			return;
		case 0x01:
			i2_RLC_C(pz80);
			return;
		case 0x02:
			i2_RLC_D(pz80);
			return;
		case 0x03:
			i2_RLC_E(pz80);
			return;
		case 0x04:
			i2_RLC_H(pz80);
			return;
		case 0x05:
			i2_RLC_L(pz80);
			return;
		case 0x06:
			i2_RLC__HL_(pz80);
			return;
		case 0x07:
			i2_RLC_A(pz80);
			return;
		case 0x08:
			i2_RRC_B(pz80);
			return;
		case 0x09:
			i2_RRC_C(pz80);
			return;
		case 0x0A:
			i2_RRC_D(pz80);
			return;
		case 0x0B:
			i2_RRC_E(pz80);
			return;
		case 0x0C:
			i2_RRC_H(pz80);
			return;
		case 0x0D:
			i2_RRC_L(pz80);
			return;
		case 0x0E:
			i2_RRC__HL_(pz80);
			return;
		case 0x0F:
			i2_RRC_A(pz80);
			return;
		case 0x10:
			i2_RL_B(pz80);
			return;
		case 0x11:
			i2_RL_C(pz80);
			return;
		case 0x12:
			i2_RL_D(pz80);
			return;
		case 0x13:
			i2_RL_E(pz80);
			return;
		case 0x14:
			i2_RL_H(pz80);
			return;
		case 0x15:
			i2_RL_L(pz80);
			return;
		case 0x16:
			i2_RL__HL_(pz80);
			return;
		case 0x17:
			i2_RL_A(pz80);
			return;
		case 0x18:
			i2_RR_B(pz80);
			return;
		case 0x19:
			i2_RR_C(pz80);
			return;
		case 0x1A:
			i2_RR_D(pz80);
			return;
		case 0x1B:
			i2_RR_E(pz80);
			return;
		case 0x1C:
			i2_RR_H(pz80);
			return;
		case 0x1D:
			i2_RR_L(pz80);
			return;
		case 0x1E:
			i2_RR__HL_(pz80);
			return;
		case 0x1F:
			i2_RR_A(pz80);
			return;
		case 0x20:
			i2_SLA_B(pz80);
			return;
		case 0x21:
			i2_SLA_C(pz80);
			return;
		case 0x22:
			i2_SLA_D(pz80);
			return;
		case 0x23:
			i2_SLA_E(pz80);
			return;
		case 0x24:
			i2_SLA_H(pz80);
			return;
		case 0x25:
			i2_SLA_L(pz80);
			return;
		case 0x26:
			i2_SLA__HL_(pz80);
			return;
		case 0x27:
			i2_SLA_A(pz80);
			return;
		case 0x28:
			i2_SRA_B(pz80);
			return;
		case 0x29:
			i2_SRA_C(pz80);
			return;
		case 0x2A:
			i2_SRA_D(pz80);
			return;
		case 0x2B:
			i2_SRA_E(pz80);
			return;
		case 0x2C:
			i2_SRA_H(pz80);
			return;
		case 0x2D:
			i2_SRA_L(pz80);
			return;
		case 0x2E:
			i2_SRA__HL_(pz80);
			return;
		case 0x2F:
			i2_SRA_A(pz80);
			return;
		case 0x30:
			i2_SWAP_B(pz80);
			return;
		case 0x31:
			i2_SWAP_C(pz80);
			return;
		case 0x32:
			i2_SWAP_D(pz80);
			return;
		case 0x33:
			i2_SWAP_E(pz80);
			return;
		case 0x34:
			i2_SWAP_H(pz80);
			return;
		case 0x35:
			i2_SWAP_L(pz80);
			return;
		case 0x36:
			i2_SWAP__HL_(pz80);
			return;
		case 0x37:
			i2_SWAP_A(pz80);
			return;
		case 0x38:
			i2_SRL_B(pz80);
			return;
		case 0x39:
			i2_SRL_C(pz80);
			return;
		case 0x3A:
			i2_SRL_D(pz80);
			return;
		case 0x3B:
			i2_SRL_E(pz80);
			return;
		case 0x3C:
			i2_SRL_H(pz80);
			return;
		case 0x3D:
			i2_SRL_L(pz80);
			return;
		case 0x3E:
			i2_SRL__HL_(pz80);
			return;
		case 0x3F:
			i2_SRL_A(pz80);
			return;
		case 0x40:
			i2_BIT_0_B(pz80);
			return;
		case 0x41:
			i2_BIT_0_C(pz80);
			return;
		case 0x42:
			i2_BIT_0_D(pz80);
			return;
		case 0x43:
			i2_BIT_0_E(pz80);
			return;
		case 0x44:
			i2_BIT_0_H(pz80);
			return;
		case 0x45:
			i2_BIT_0_L(pz80);
			return;
		case 0x46:
			i2_BIT_0__HL_(pz80);
			return;
		case 0x47:
			i2_BIT_0_A(pz80);
			return;
		case 0x48:
			i2_BIT_1_B(pz80);
			return;
		case 0x49:
			i2_BIT_1_C(pz80);
			return;
		case 0x4A:
			i2_BIT_1_D(pz80);
			return;
		case 0x4B:
			i2_BIT_1_E(pz80);
			return;
		case 0x4C:
			i2_BIT_1_H(pz80);
			return;
		case 0x4D:
			i2_BIT_1_L(pz80);
			return;
		case 0x4E:
			i2_BIT_1__HL_(pz80);
			return;
		case 0x4F:
			i2_BIT_1_A(pz80);
			return;
		case 0x50:
			i2_BIT_2_B(pz80);
			return;
		case 0x51:
			i2_BIT_2_C(pz80);
			return;
		case 0x52:
			i2_BIT_2_D(pz80);
			return;
		case 0x53:
			i2_BIT_2_E(pz80);
			return;
		case 0x54:
			i2_BIT_2_H(pz80);
			return;
		case 0x55:
			i2_BIT_2_L(pz80);
			return;
		case 0x56:
			i2_BIT_2__HL_(pz80);
			return;
		case 0x57:
			i2_BIT_2_A(pz80);
			return;
		case 0x58:
			i2_BIT_3_B(pz80);
			return;
		case 0x59:
			i2_BIT_3_C(pz80);
			return;
		case 0x5A:
			i2_BIT_3_D(pz80);
			return;
		case 0x5B:
			i2_BIT_3_E(pz80);
			return;
		case 0x5C:
			i2_BIT_3_H(pz80);
			return;
		case 0x5D:
			i2_BIT_3_L(pz80);
			return;
		case 0x5E:
			i2_BIT_3__HL_(pz80);
			return;
		case 0x5F:
			i2_BIT_3_A(pz80);
			return;
		case 0x60:
			i2_BIT_4_B(pz80);
			return;
		case 0x61:
			i2_BIT_4_C(pz80);
			return;
		case 0x62:
			i2_BIT_4_D(pz80);
			return;
		case 0x63:
			i2_BIT_4_E(pz80);
			return;
		case 0x64:
			i2_BIT_4_H(pz80);
			return;
		case 0x65:
			i2_BIT_4_L(pz80);
			return;
		case 0x66:
			i2_BIT_4__HL_(pz80);
			return;
		case 0x67:
			i2_BIT_4_A(pz80);
			return;
		case 0x68:
			i2_BIT_5_B(pz80);
			return;
		case 0x69:
			i2_BIT_5_C(pz80);
			return;
		case 0x6A:
			i2_BIT_5_D(pz80);
			return;
		case 0x6B:
			i2_BIT_5_E(pz80);
			return;
		case 0x6C:
			i2_BIT_5_H(pz80);
			return;
		case 0x6D:
			i2_BIT_5_L(pz80);
			return;
		case 0x6E:
			i2_BIT_5__HL_(pz80);
			return;
		case 0x6F:
			i2_BIT_5_A(pz80);
			return;
		case 0x70:
			i2_BIT_6_B(pz80);
			return;
		case 0x71:
			i2_BIT_6_C(pz80);
			return;
		case 0x72:
			i2_BIT_6_D(pz80);
			return;
		case 0x73:
			i2_BIT_6_E(pz80);
			return;
		case 0x74:
			i2_BIT_6_H(pz80);
			return;
		case 0x75:
			i2_BIT_6_L(pz80);
			return;
		case 0x76:
			i2_BIT_6__HL_(pz80);
			return;
		case 0x77:
			i2_BIT_6_A(pz80);
			return;
		case 0x78:
			i2_BIT_7_B(pz80);
			return;
		case 0x79:
			i2_BIT_7_C(pz80);
			return;
		case 0x7A:
			i2_BIT_7_D(pz80);
			return;
		case 0x7B:
			i2_BIT_7_E(pz80);
			return;
		case 0x7C:
			i2_BIT_7_H(pz80);
			return;
		case 0x7D:
			i2_BIT_7_L(pz80);
			return;
		case 0x7E:
			i2_BIT_7__HL_(pz80);
			return;
		case 0x7F:
			i2_BIT_7_A(pz80);
			return;
		case 0x80:
			i2_RES_0_B(pz80);
			return;
		case 0x81:
			i2_RES_0_C(pz80);
			return;
		case 0x82:
			i2_RES_0_D(pz80);
			return;
		case 0x83:
			i2_RES_0_E(pz80);
			return;
		case 0x84:
			i2_RES_0_H(pz80);
			return;
		case 0x85:
			i2_RES_0_L(pz80);
			return;
		case 0x86:
			i2_RES_0__HL_(pz80);
			return;
		case 0x87:
			i2_RES_0_A(pz80);
			return;
		case 0x88:
			i2_RES_1_B(pz80);
			return;
		case 0x89:
			i2_RES_1_C(pz80);
			return;
		case 0x8A:
			i2_RES_1_D(pz80);
			return;
		case 0x8B:
			i2_RES_1_E(pz80);
			return;
		case 0x8C:
			i2_RES_1_H(pz80);
			return;
		case 0x8D:
			i2_RES_1_L(pz80);
			return;
		case 0x8E:
			i2_RES_1__HL_(pz80);
			return;
		case 0x8F:
			i2_RES_1_A(pz80);
			return;
		case 0x90:
			i2_RES_2_B(pz80);
			return;
		case 0x91:
			i2_RES_2_C(pz80);
			return;
		case 0x92:
			i2_RES_2_D(pz80);
			return;
		case 0x93:
			i2_RES_2_E(pz80);
			return;
		case 0x94:
			i2_RES_2_H(pz80);
			return;
		case 0x95:
			i2_RES_2_L(pz80);
			return;
		case 0x96:
			i2_RES_2__HL_(pz80);
			return;
		case 0x97:
			i2_RES_2_A(pz80);
			return;
		case 0x98:
			i2_RES_3_B(pz80);
			return;
		case 0x99:
			i2_RES_3_C(pz80);
			return;
		case 0x9A:
			i2_RES_3_D(pz80);
			return;
		case 0x9B:
			i2_RES_3_E(pz80);
			return;
		case 0x9C:
			i2_RES_3_H(pz80);
			return;
		case 0x9D:
			i2_RES_3_L(pz80);
			return;
		case 0x9E:
			i2_RES_3__HL_(pz80);
			return;
		case 0x9F:
			i2_RES_3_A(pz80);
			return;
		case 0xA0:
			i2_RES_4_B(pz80);
			return;
		case 0xA1:
			i2_RES_4_C(pz80);
			return;
		case 0xA2:
			i2_RES_4_D(pz80);
			return;
		case 0xA3:
			i2_RES_4_E(pz80);
			return;
		case 0xA4:
			i2_RES_4_H(pz80);
			return;
		case 0xA5:
			i2_RES_4_L(pz80);
			return;
		case 0xA6:
			i2_RES_4__HL_(pz80);
			return;
		case 0xA7:
			i2_RES_4_A(pz80);
			return;
		case 0xA8:
			i2_RES_5_B(pz80);
			return;
		case 0xA9:
			i2_RES_5_C(pz80);
			return;
		case 0xAA:
			i2_RES_5_D(pz80);
			return;
		case 0xAB:
			i2_RES_5_E(pz80);
			return;
		case 0xAC:
			i2_RES_5_H(pz80);
			return;
		case 0xAD:
			i2_RES_5_L(pz80);
			return;
		case 0xAE:
			i2_RES_5__HL_(pz80);
			return;
		case 0xAF:
			i2_RES_5_A(pz80);
			return;
		case 0xB0:
			i2_RES_6_B(pz80);
			return;
		case 0xB1:
			i2_RES_6_C(pz80);
			return;
		case 0xB2:
			i2_RES_6_D(pz80);
			return;
		case 0xB3:
			i2_RES_6_E(pz80);
			return;
		case 0xB4:
			i2_RES_6_H(pz80);
			return;
		case 0xB5:
			i2_RES_6_L(pz80);
			return;
		case 0xB6:
			i2_RES_6__HL_(pz80);
			return;
		case 0xB7:
			i2_RES_6_A(pz80);
			return;
		case 0xB8:
			i2_RES_7_B(pz80);
			return;
		case 0xB9:
			i2_RES_7_C(pz80);
			return;
		case 0xBA:
			i2_RES_7_D(pz80);
			return;
		case 0xBB:
			i2_RES_7_E(pz80);
			return;
		case 0xBC:
			i2_RES_7_H(pz80);
			return;
		case 0xBD:
			i2_RES_7_L(pz80);
			return;
		case 0xBE:
			i2_RES_7__HL_(pz80);
			return;
		case 0xBF:
			i2_RES_7_A(pz80);
			return;
		case 0xC0:
			i2_SET_0_B(pz80);
			return;
		case 0xC1:
			i2_SET_0_C(pz80);
			return;
		case 0xC2:
			i2_SET_0_D(pz80);
			return;
		case 0xC3:
			i2_SET_0_E(pz80);
			return;
		case 0xC4:
			i2_SET_0_H(pz80);
			return;
		case 0xC5:
			i2_SET_0_L(pz80);
			return;
		case 0xC6:
			i2_SET_0__HL_(pz80);
			return;
		case 0xC7:
			i2_SET_0_A(pz80);
			return;
		case 0xC8:
			i2_SET_1_B(pz80);
			return;
		case 0xC9:
			i2_SET_1_C(pz80);
			return;
		case 0xCA:
			i2_SET_1_D(pz80);
			return;
		case 0xCB:
			i2_SET_1_E(pz80);
			return;
		case 0xCC:
			i2_SET_1_H(pz80);
			return;
		case 0xCD:
			i2_SET_1_L(pz80);
			return;
		case 0xCE:
			i2_SET_1__HL_(pz80);
			return;
		case 0xCF:
			i2_SET_1_A(pz80);
			return;
		case 0xD0:
			i2_SET_2_B(pz80);
			return;
		case 0xD1:
			i2_SET_2_C(pz80);
			return;
		case 0xD2:
			i2_SET_2_D(pz80);
			return;
		case 0xD3:
			i2_SET_2_E(pz80);
			return;
		case 0xD4:
			i2_SET_2_H(pz80);
			return;
		case 0xD5:
			i2_SET_2_L(pz80);
			return;
		case 0xD6:
			i2_SET_2__HL_(pz80);
			return;
		case 0xD7:
			i2_SET_2_A(pz80);
			return;
		case 0xD8:
			i2_SET_3_B(pz80);
			return;
		case 0xD9:
			i2_SET_3_C(pz80);
			return;
		case 0xDA:
			i2_SET_3_D(pz80);
			return;
		case 0xDB:
			i2_SET_3_E(pz80);
			return;
		case 0xDC:
			i2_SET_3_H(pz80);
			return;
		case 0xDD:
			i2_SET_3_L(pz80);
			return;
		case 0xDE:
			i2_SET_3__HL_(pz80);
			return;
		case 0xDF:
			i2_SET_3_A(pz80);
			return;
		case 0xE0:
			i2_SET_4_B(pz80);
			return;
		case 0xE1:
			i2_SET_4_C(pz80);
			return;
		case 0xE2:
			i2_SET_4_D(pz80);
			return;
		case 0xE3:
			i2_SET_4_E(pz80);
			return;
		case 0xE4:
			i2_SET_4_H(pz80);
			return;
		case 0xE5:
			i2_SET_4_L(pz80);
			return;
		case 0xE6:
			i2_SET_4__HL_(pz80);
			return;
		case 0xE7:
			i2_SET_4_A(pz80);
			return;
		case 0xE8:
			i2_SET_5_B(pz80);
			return;
		case 0xE9:
			i2_SET_5_C(pz80);
			return;
		case 0xEA:
			i2_SET_5_D(pz80);
			return;
		case 0xEB:
			i2_SET_5_E(pz80);
			return;
		case 0xEC:
			i2_SET_5_H(pz80);
			return;
		case 0xED:
			i2_SET_5_L(pz80);
			return;
		case 0xEE:
			i2_SET_5__HL_(pz80);
			return;
		case 0xEF:
			i2_SET_5_A(pz80);
			return;
		case 0xF0:
			i2_SET_6_B(pz80);
			return;
		case 0xF1:
			i2_SET_6_C(pz80);
			return;
		case 0xF2:
			i2_SET_6_D(pz80);
			return;
		case 0xF3:
			i2_SET_6_E(pz80);
			return;
		case 0xF4:
			i2_SET_6_H(pz80);
			return;
		case 0xF5:
			i2_SET_6_L(pz80);
			return;
		case 0xF6:
			i2_SET_6__HL_(pz80);
			return;
		case 0xF7:
			i2_SET_6_A(pz80);
			return;
		case 0xF8:
			i2_SET_7_B(pz80);
			return;
		case 0xF9:
			i2_SET_7_C(pz80);
			return;
		case 0xFA:
			i2_SET_7_D(pz80);
			return;
		case 0xFB:
			i2_SET_7_E(pz80);
			return;
		case 0xFC:
			i2_SET_7_H(pz80);
			return;
		case 0xFD:
			i2_SET_7_L(pz80);
			return;
		case 0xFE:
			i2_SET_7__HL_(pz80);
			return;
		case 0xFF:
			i2_SET_7_A(pz80);
			return;
	}


}



/********************/
/* No Operation */
void
i_NOP(z80 * pz80){
	incPC(pz80,1);
	pz80->tcycles = 4;
}
/* Load 16-bit immediate into BC */
void
i_LD_BC_nn(z80 * pz80){
LD_nn_immediate(pz80, REGB, REGC);
}
/* Save A to address pointed by BC */
void
i_LD__BC__A(z80 * pz80){
LD_nn_mem_n(pz80, REGB, REGC, REGA);
}
/* Increment 16-bit BC */
void
i_INC_BC(z80 * pz80){
INC_nn(pz80, REGB, REGC);
}
/* Increment B */
void
i_INC_B(z80 * pz80){
INC_n(pz80, REGB);
}
/* Decrement B */
void
i_DEC_B(z80 * pz80){
DEC_n(pz80, REGB);
}
/* Load 8-bit immediate into B */
void
i_LD_B_n(z80 * pz80){
LD_n_immediate(pz80, REGB);
}
/* Rotate A left with carry */
void
i_RLC_A(z80 * pz80){
RLC_n(pz80,REGA);
}
/* Save SP to given address */
void
i_LD__nn__SP(z80 * pz80){
LD_nn_SP(pz80);
}
/* Add 16-bit BC to HL */
void
i_ADD_HL_BC(z80 * pz80){
ADD_HL_nn(pz80, REGB, REGC);
}
/* Load A from address pointed to by BC */
void
i_LD_A__BC_(z80 * pz80){
LD_n_nn_mem(pz80, REGA, REGB, REGC);
}
/* Decrement 16-bit BC */
void
i_DEC_BC(z80 * pz80){
DEC_nn(pz80, REGB, REGC);
}
/* Increment C */
void
i_INC_C(z80 * pz80){
INC_n(pz80, REGC);
}
/* Decrement C */
void
i_DEC_C(z80 * pz80){
DEC_n(pz80, REGC);
}
/* Load 8-bit immediate into C */
void
i_LD_C_n(z80 * pz80){
LD_n_immediate(pz80, REGC);
}
/* Rotate A right with carry */
void
i_RRC_A(z80 * pz80){
RRC_n(pz80,REGA);
}
/* Stop processor */
void
i_STOP(z80 * pz80){
}
/* Load 16-bit immediate into DE */
void
i_LD_DE_nn(z80 * pz80){
LD_nn_immediate(pz80, REGD, REGE);
}
/* Save A to address pointed by DE */
void
i_LD__DE__A(z80 * pz80){
LD_nn_mem_n(pz80, REGD, REGE, REGA);
}
/* Increment 16-bit DE */
void
i_INC_DE(z80 * pz80){
INC_nn(pz80, REGD, REGE);
}
/* Increment D */
void
i_INC_D(z80 * pz80){
INC_n(pz80, REGD);
}
/* Decrement D */
void
i_DEC_D(z80 * pz80){
DEC_n(pz80, REGD);
}
/* Load 8-bit immediate into D */
void
i_LD_D_n(z80 * pz80){
LD_n_immediate(pz80, REGD);
}
/* Rotate A left */
void
i_RL_A(z80 * pz80){
RL_n(pz80,REGA);
}
/* Relative jump by signed immediate */
void
i_JR_n(z80 * pz80){
JR_n(pz80);
}
/* Add 16-bit DE to HL */
void
i_ADD_HL_DE(z80 * pz80){
ADD_HL_nn(pz80, REGD, REGE);
}
/* Load A from address pointed to by DE */
void
i_LD_A__DE_(z80 * pz80){
LD_n_nn_mem(pz80, REGA, REGD, REGE);
}
/* Decrement 16-bit DE */
void
i_DEC_DE(z80 * pz80){
DEC_nn(pz80, REGD, REGE);
}
/* Increment E */
void
i_INC_E(z80 * pz80){
INC_n(pz80, REGE);
}
/* Decrement E */
void
i_DEC_E(z80 * pz80){
DEC_n(pz80, REGE);
}
/* Load 8-bit immediate into E */
void
i_LD_E_n(z80 * pz80){
LD_n_immediate(pz80, REGE);
}
/* Rotate A right */
void
i_RR_A(z80 * pz80){
RR_n(pz80,REGA);
}
/* Relative jump by signed immediate if last result was not zero */
void
i_JR_NZ_n(z80 * pz80){
JR_NZ_n(pz80);
}
/* Load 16-bit immediate into HL */
void
i_LD_HL_nn(z80 * pz80){
LD_nn_immediate(pz80, REGH, REGL);
}
/* Save A to address pointed by HL, and increment HL */
void
i_LDI__HL__A(z80 * pz80){
LDI_HL_mem_A(pz80);
}
/* Increment 16-bit HL */
void
i_INC_HL(z80 * pz80){
INC_nn(pz80, REGH, REGL);
}
/* Increment H */
void
i_INC_H(z80 * pz80){
INC_n(pz80, REGH);
}
/* Decrement H */
void
i_DEC_H(z80 * pz80){
DEC_n(pz80, REGH);
}
/* Load 8-bit immediate into H */
void
i_LD_H_n(z80 * pz80){
LD_n_immediate(pz80, REGH);
}
/* Adjust A for BCD addition */
void
i_DAA(z80 * pz80){
uint8_t oldA = pz80->registers[REGA];
if((pz80->registers[REGA] & 0xf) > 9)
	pz80->registers[REGA] += 6;
pz80->registers[REGF] = buildStatusFlag((pz80->registers[REGA] == 0), getFlag(pz80->registers[REGF], SUB), 0, (oldA > pz80->registers[REGA]));
pz80->tcycles = 4;
incPC(pz80, 1);
}
/* Relative jump by signed immediate if last result was zero */
void
i_JR_Z_n(z80 * pz80){
JR_Z_n(pz80);
}
/* Add 16-bit HL to HL */
void
i_ADD_HL_HL(z80 * pz80){
ADD_HL_nn(pz80, REGH, REGL);
}
/* Load A from address pointed to by HL, and increment HL */
void
i_LDI_A__HL_(z80 * pz80){
LDI_A_HL_mem(pz80);
}
/* Decrement 16-bit HL */
void
i_DEC_HL(z80 * pz80){
DEC_nn(pz80, REGH, REGL);
}
/* Increment L */
void
i_INC_L(z80 * pz80){
INC_n(pz80, REGL);
}
/* Decrement L */
void
i_DEC_L(z80 * pz80){
DEC_n(pz80, REGL);
}
/* Load 8-bit immediate into L */
void
i_LD_L_n(z80 * pz80){
LD_n_immediate(pz80, REGL);
}
/* Complement (logical NOT) on A */
void
i_CPL(z80 * pz80){
pz80->registers[REGA] = ~pz80->registers[REGA];
pz80->tcycles = 4;
incPC(pz80, 1);
}
/* Relative jump by signed immediate if last result caused no carry */
void
i_JR_NC_n(z80 * pz80){
JR_NC_n(pz80);
}
/* Load 16-bit immediate into SP */
void
i_LD_SP_nn(z80 * pz80){
LD_SP_nn(pz80);
}
/* Save A to address pointed by HL, and decrement HL */
void
i_LDD__HL__A(z80 * pz80){
LDD_HL_mem_A(pz80);
}
/* Increment 16-bit HL */
void
i_INC_SP(z80 * pz80){
pz80->registers16[SP]++;
pz80->registers[REGF] = buildStatusFlag((pz80->registers16[SP] == 0), 0, ((pz80->registers16[SP] & 0xffff) == 0), getFlag(pz80->registers[REGF], CARRY));
pz80->tcycles = 8;
incPC(pz80, 1);
}
/* Increment value pointed by HL */
void
i_INC__HL_(z80 * pz80){
uint8_t newVal = rb(&(pz80->mmu),(pz80->registers[REGH] << 8) + pz80->registers[REGL]) + 1;
wb(&(pz80->mmu),(pz80->registers[REGH] << 8) + pz80->registers[REGL], newVal);
pz80->registers[REGF] = buildStatusFlag((newVal == 0), 0, ((newVal & 0xff) == 0), getFlag(pz80->registers[REGF], CARRY));
pz80->tcycles = 12;
incPC(pz80, 1);
}
/* Decrement value pointed by HL */
void
i_DEC__HL_(z80 * pz80){
uint8_t newVal = rb(&(pz80->mmu),(pz80->registers[REGH] << 8) + pz80->registers[REGL]) - 1;
wb(&(pz80->mmu),(pz80->registers[REGH] << 8) + pz80->registers[REGL], newVal);
pz80->registers[REGF] = buildStatusFlag((newVal == 0), 0, ((newVal & 0xff) == 0xff), getFlag(pz80->registers[REGF], CARRY));
pz80->tcycles = 12;
incPC(pz80, 1);
}
/* Load 8-bit immediate into address pointed by HL */
void
i_LD__HL__n(z80 * pz80){
LD_nn_mem_immediate(pz80, REGH, REGL);
}
/* Set carry flag */
void
i_SCF(z80 * pz80){
pz80->registers[REGF] = buildStatusFlag(getFlag(pz80->registers[REGF], ZERO), 0, 0, 1);
pz80->tcycles = 4;
incPC(pz80, 1);
}
/* Relative jump by signed immediate if last result caused carry */
void
i_JR_C_n(z80 * pz80){
JR_C_n(pz80);
}
/* Add 16-bit SP to HL */
void
i_ADD_HL_SP(z80 * pz80){
ADD_HL_SP(pz80);
}
/* Load A from address pointed to by HL, and decrement HL */
void
i_LDD_A__HL_(z80 * pz80){
LDD_A_HL_mem(pz80);
}
/* Decrement 16-bit SP */
void
i_DEC_SP(z80 * pz80){
pz80->registers16[SP]--;
pz80->registers[REGF] = buildStatusFlag((pz80->registers16[SP] == 0), 0, ((pz80->registers16[SP] & 0xffff) == 0xffff), getFlag(pz80->registers[REGF], CARRY));
}
/* Increment A */
void
i_INC_A(z80 * pz80){
INC_n(pz80, REGA);
}
/* Decrement A */
void
i_DEC_A(z80 * pz80){
DEC_n(pz80, REGA);
}
/* Load 8-bit immediate into A */
void
i_LD_A_n(z80 * pz80){
LD_n_immediate(pz80, REGA);
}
/* Clear carry flag */
void
i_CCF(z80 * pz80){
pz80->registers[REGF] = buildStatusFlag(getFlag(pz80->registers[REGF], ZERO), 0, 0, !getFlag(pz80->registers[REGF], CARRY));
pz80->tcycles = 4;
incPC(pz80, 1);
}
/* Copy B to B */
void
i_LD_B_B(z80 * pz80){
LD_n_n(pz80, REGB, REGB);
}
/* Copy C to B */
void
i_LD_B_C(z80 * pz80){
LD_n_n(pz80, REGB, REGC);
}
/* Copy D to B */
void
i_LD_B_D(z80 * pz80){
LD_n_n(pz80, REGB, REGD);
}
/* Copy E to B */
void
i_LD_B_E(z80 * pz80){
LD_n_n(pz80, REGB, REGE);
}
/* Copy H to B */
void
i_LD_B_H(z80 * pz80){
LD_n_n(pz80, REGB, REGH);
}
/* Copy L to B */
void
i_LD_B_L(z80 * pz80){
LD_n_n(pz80, REGB, REGL);
}
/* Copy value pointed by HL to B */
void
i_LD_B__HL_(z80 * pz80){
LD_n_nn_mem(pz80, REGB, REGH, REGL);
}
/* Copy A to B */
void
i_LD_B_A(z80 * pz80){
LD_n_n(pz80, REGB, REGA);
}
/* Copy B to C */
void
i_LD_C_B(z80 * pz80){
LD_n_n(pz80, REGC, REGB);
}
/* Copy C to C */
void
i_LD_C_C(z80 * pz80){
LD_n_n(pz80, REGC, REGC);
}
/* Copy D to C */
void
i_LD_C_D(z80 * pz80){
LD_n_n(pz80, REGC, REGD);
}
/* Copy E to C */
void
i_LD_C_E(z80 * pz80){
LD_n_n(pz80, REGC, REGE);
}
/* Copy H to C */
void
i_LD_C_H(z80 * pz80){
LD_n_n(pz80, REGC, REGH);
}
/* Copy L to C */
void
i_LD_C_L(z80 * pz80){
LD_n_n(pz80, REGC, REGL);
}
/* Copy value pointed by HL to C */
void
i_LD_C__HL_(z80 * pz80){
LD_n_nn_mem(pz80, REGC, REGH, REGL);
}
/* Copy A to C */
void
i_LD_C_A(z80 * pz80){
LD_n_n(pz80, REGC, REGA);
}
/* Copy B to D */
void
i_LD_D_B(z80 * pz80){
LD_n_n(pz80, REGD, REGB);
}
/* Copy C to D */
void
i_LD_D_C(z80 * pz80){
LD_n_n(pz80, REGD, REGC);
}
/* Copy D to D */
void
i_LD_D_D(z80 * pz80){
LD_n_n(pz80, REGD, REGD);
}
/* Copy E to D */
void
i_LD_D_E(z80 * pz80){
LD_n_n(pz80, REGD, REGE);
}
/* Copy H to D */
void
i_LD_D_H(z80 * pz80){
LD_n_n(pz80, REGD, REGH);
}
/* Copy L to D */
void
i_LD_D_L(z80 * pz80){
LD_n_n(pz80, REGD, REGL);
}
/* Copy value pointed by HL to D */
void
i_LD_D__HL_(z80 * pz80){
LD_n_nn_mem(pz80, REGD, REGH, REGL);
}
/* Copy A to D */
void
i_LD_D_A(z80 * pz80){
LD_n_n(pz80, REGD, REGA);
}
/* Copy B to E */
void
i_LD_E_B(z80 * pz80){
LD_n_n(pz80, REGE, REGB);
}
/* Copy C to E */
void
i_LD_E_C(z80 * pz80){
LD_n_n(pz80, REGE, REGC);
}
/* Copy D to E */
void
i_LD_E_D(z80 * pz80){
LD_n_n(pz80, REGE, REGD);
}
/* Copy E to E */
void
i_LD_E_E(z80 * pz80){
LD_n_n(pz80, REGE, REGE);
}
/* Copy H to E */
void
i_LD_E_H(z80 * pz80){
LD_n_n(pz80, REGE, REGH);
}
/* Copy L to E */
void
i_LD_E_L(z80 * pz80){
LD_n_n(pz80, REGE, REGL);
}
/* Copy value pointed by HL to E */
void
i_LD_E__HL_(z80 * pz80){
LD_n_nn_mem(pz80, REGE, REGH, REGL);
}
/* Copy A to E */
void
i_LD_E_A(z80 * pz80){
LD_n_n(pz80, REGE, REGA);
}
/* Copy B to H */
void
i_LD_H_B(z80 * pz80){
LD_n_n(pz80, REGH, REGB);
}
/* Copy C to H */
void
i_LD_H_C(z80 * pz80){
LD_n_n(pz80, REGH, REGC);
}
/* Copy D to H */
void
i_LD_H_D(z80 * pz80){
LD_n_n(pz80, REGH, REGD);
}
/* Copy E to H */
void
i_LD_H_E(z80 * pz80){
LD_n_n(pz80, REGH, REGE);
}
/* Copy H to H */
void
i_LD_H_H(z80 * pz80){
LD_n_n(pz80, REGH, REGH);
}
/* Copy L to H */
void
i_LD_H_L(z80 * pz80){
LD_n_n(pz80, REGH, REGL);
}
/* Copy value pointed by HL to H */
void
i_LD_H__HL_(z80 * pz80){
LD_n_nn_mem(pz80, REGH, REGH, REGL);
}
/* Copy A to H */
void
i_LD_H_A(z80 * pz80){
LD_n_n(pz80, REGH, REGA);
}
/* Copy B to L */
void
i_LD_L_B(z80 * pz80){
LD_n_n(pz80, REGL, REGB);
}
/* Copy C to L */
void
i_LD_L_C(z80 * pz80){
LD_n_n(pz80, REGL, REGC);
}
/* Copy D to L */
void
i_LD_L_D(z80 * pz80){
LD_n_n(pz80, REGL, REGD);
}
/* Copy E to L */
void
i_LD_L_E(z80 * pz80){
LD_n_n(pz80, REGL, REGE);
}
/* Copy H to L */
void
i_LD_L_H(z80 * pz80){
LD_n_n(pz80, REGL, REGH);
}
/* Copy L to L */
void
i_LD_L_L(z80 * pz80){
LD_n_n(pz80, REGL, REGL);
}
/* Copy value pointed by HL to L */
void
i_LD_L__HL_(z80 * pz80){
LD_n_nn_mem(pz80, REGL, REGH, REGL);
}
/* Copy A to L */
void
i_LD_L_A(z80 * pz80){
LD_n_n(pz80, REGL, REGA);
}
/* Copy B to address pointed by HL */
void
i_LD__HL__B(z80 * pz80){
LD_nn_mem_n(pz80, REGH, REGL, REGB);
}
/* Copy C to address pointed by HL */
void
i_LD__HL__C(z80 * pz80){
LD_nn_mem_n(pz80, REGH, REGL, REGC);
}
/* Copy D to address pointed by HL */
void
i_LD__HL__D(z80 * pz80){
LD_nn_mem_n(pz80, REGH, REGL, REGD);
}
/* Copy E to address pointed by HL */
void
i_LD__HL__E(z80 * pz80){
LD_nn_mem_n(pz80, REGH, REGL, REGE);
}
/* Copy H to address pointed by HL */
void
i_LD__HL__H(z80 * pz80){
LD_nn_mem_n(pz80, REGH, REGL, REGH);
}
/* Copy L to address pointed by HL */
void
i_LD__HL__L(z80 * pz80){
LD_nn_mem_n(pz80, REGH, REGL, REGL);
}
/* Halt processor */
void
i_HALT(z80 * pz80){
}
/* Copy A to address pointed by HL */
void
i_LD__HL__A(z80 * pz80){
LD_nn_mem_n(pz80, REGH, REGL, REGA);
}
/* Copy B to A */
void
i_LD_A_B(z80 * pz80){
LD_n_n(pz80, REGA, REGB);
}
/* Copy C to A */
void
i_LD_A_C(z80 * pz80){
LD_n_n(pz80, REGA, REGC);
}
/* Copy D to A */
void
i_LD_A_D(z80 * pz80){
LD_n_n(pz80, REGA, REGD);
}
/* Copy E to A */
void
i_LD_A_E(z80 * pz80){
LD_n_n(pz80, REGA, REGE);
}
/* Copy H to A */
void
i_LD_A_H(z80 * pz80){
LD_n_n(pz80, REGA, REGH);
}
/* Copy L to A */
void
i_LD_A_L(z80 * pz80){
LD_n_n(pz80, REGA, REGL);
}
/* Copy value pointed by HL to A */
void
i_LD_A__HL_(z80 * pz80){
LD_n_nn_mem(pz80, REGA, REGH, REGL);
}
/* Copy A to A */
void
i_LD_A_A(z80 * pz80){
LD_n_n(pz80, REGA, REGA);
}
/* Add B to A */
void
i_ADD_A_B(z80 * pz80){
ADD_A_n(pz80, REGB);
}
/* Add C to A */
void
i_ADD_A_C(z80 * pz80){
ADD_A_n(pz80, REGC);
}
/* Add D to A */
void
i_ADD_A_D(z80 * pz80){
ADD_A_n(pz80, REGD);
}
/* Add E to A */
void
i_ADD_A_E(z80 * pz80){
ADD_A_n(pz80, REGE);
}
/* Add H to A */
void
i_ADD_A_H(z80 * pz80){
ADD_A_n(pz80, REGH);
}
/* Add L to A */
void
i_ADD_A_L(z80 * pz80){
ADD_A_n(pz80, REGL);
}
/* Add value pointed by HL to A */
void
i_ADD_A__HL_(z80 * pz80){
ADD_A_nn_mem(pz80, REGH, REGL);
}
/* Add A to A */
void
i_ADD_A_A(z80 * pz80){
ADD_A_n(pz80, REGA);
}
/* Add B and carry flag to A */
void
i_ADC_A_B(z80 * pz80){
ADC_A_n(pz80, REGB);
}
/* Add C and carry flag to A */
void
i_ADC_A_C(z80 * pz80){
ADC_A_n(pz80, REGC);
}
/* Add D and carry flag to A */
void
i_ADC_A_D(z80 * pz80){
ADC_A_n(pz80, REGD);
}
/* Add E and carry flag to A */
void
i_ADC_A_E(z80 * pz80){
ADC_A_n(pz80, REGE);
}
/* Add H and carry flag to A */
void
i_ADC_A_H(z80 * pz80){
ADC_A_n(pz80, REGH);
}
/* Add and carry flag L to A */
void
i_ADC_A_L(z80 * pz80){
ADC_A_n(pz80, REGL);
}
/* Add value pointed by HL and carry flag to A */
void
i_ADC_A__HL_(z80 * pz80){
ADC_A_nn_mem(pz80, REGH, REGL);
}
/* Add A and carry flag to A */
void
i_ADC_A_A(z80 * pz80){
ADC_A_n(pz80, REGA);
}
/* Subtract B from A */
void
i_SUB_A_B(z80 * pz80){
SUB_A_n(pz80, REGB);
}
/* Subtract C from A */
void
i_SUB_A_C(z80 * pz80){
SUB_A_n(pz80, REGC);
}
/* Subtract D from A */
void
i_SUB_A_D(z80 * pz80){
SUB_A_n(pz80, REGD);
}
/* Subtract E from A */
void
i_SUB_A_E(z80 * pz80){
SUB_A_n(pz80, REGE);
}
/* Subtract H from A */
void
i_SUB_A_H(z80 * pz80){
SUB_A_n(pz80, REGH);
}
/* Subtract L from A */
void
i_SUB_A_L(z80 * pz80){
SUB_A_n(pz80, REGL);
}
/* Subtract value pointed by HL from A */
void
i_SUB_A__HL_(z80 * pz80){
SUB_A_HL_mem(pz80);
}
/* Subtract A from A */
void
i_SUB_A_A(z80 * pz80){
SUB_A_n(pz80, REGA);
}
/* Subtract B and carry flag from A */
void
i_SBC_A_B(z80 * pz80){
SBC_A_n(pz80, REGB);
}
/* Subtract C and carry flag from A */
void
i_SBC_A_C(z80 * pz80){
SBC_A_n(pz80, REGC);
}
/* Subtract D and carry flag from A */
void
i_SBC_A_D(z80 * pz80){
SBC_A_n(pz80, REGD);
}
/* Subtract E and carry flag from A */
void
i_SBC_A_E(z80 * pz80){
SBC_A_n(pz80, REGE);
}
/* Subtract H and carry flag from A */
void
i_SBC_A_H(z80 * pz80){
SBC_A_n(pz80, REGH);
}
/* Subtract and carry flag L from A */
void
i_SBC_A_L(z80 * pz80){
SBC_A_n(pz80, REGL);
}
/* Subtract value pointed by HL and carry flag from A */
void
i_SBC_A__HL_(z80 * pz80){
SBC_A_HL_mem(pz80);
}
/* Subtract A and carry flag from A */
void
i_SBC_A_A(z80 * pz80){
SBC_A_n(pz80, REGA);
}
/* Logical AND B against A */
void
i_AND_B(z80 * pz80){
AND_A_n(pz80, REGB);
}
/* Logical AND C against A */
void
i_AND_C(z80 * pz80){
AND_A_n(pz80, REGC);
}
/* Logical AND D against A */
void
i_AND_D(z80 * pz80){
AND_A_n(pz80, REGD);
}
/* Logical AND E against A */
void
i_AND_E(z80 * pz80){
AND_A_n(pz80, REGE);
}
/* Logical AND H against A */
void
i_AND_H(z80 * pz80){
AND_A_n(pz80, REGH);
}
/* Logical AND L against A */
void
i_AND_L(z80 * pz80){
AND_A_n(pz80, REGL);
}
/* Logical AND value pointed by HL against A */
void
i_AND__HL_(z80 * pz80){
AND_A_HL_mem(pz80);
}
/* Logical AND A against A */
void
i_AND_A(z80 * pz80){
AND_A_n(pz80, REGA);
}
/* Logical XOR B against A */
void
i_XOR_B(z80 * pz80){
XOR_A_n(pz80, REGB);
}
/* Logical XOR C against A */
void
i_XOR_C(z80 * pz80){
XOR_A_n(pz80, REGC);
}
/* Logical XOR D against A */
void
i_XOR_D(z80 * pz80){
XOR_A_n(pz80, REGD);
}
/* Logical XOR E against A */
void
i_XOR_E(z80 * pz80){
XOR_A_n(pz80, REGE);
}
/* Logical XOR H against A */
void
i_XOR_H(z80 * pz80){
XOR_A_n(pz80, REGH);
}
/* Logical XOR L against A */
void
i_XOR_L(z80 * pz80){
XOR_A_n(pz80, REGL);
}
/* Logical XOR value pointed by HL against A */
void
i_XOR__HL_(z80 * pz80){
XOR_A_HL_mem(pz80);
}
/* Logical XOR A against A */
void
i_XOR_A(z80 * pz80){
XOR_A_n(pz80, REGA);
}
/* Logical OR B against A */
void
i_OR_B(z80 * pz80){
OR_A_n(pz80, REGB);
}
/* Logical OR C against A */
void
i_OR_C(z80 * pz80){
OR_A_n(pz80, REGC);
}
/* Logical OR D against A */
void
i_OR_D(z80 * pz80){
OR_A_n(pz80, REGD);
}
/* Logical OR E against A */
void
i_OR_E(z80 * pz80){
OR_A_n(pz80, REGE);
}
/* Logical OR H against A */
void
i_OR_H(z80 * pz80){
OR_A_n(pz80, REGH);
}
/* Logical OR L against A */
void
i_OR_L(z80 * pz80){
OR_A_n(pz80, REGL);
}
/* Logical OR value pointed by HL against A */
void
i_OR__HL_(z80 * pz80){
OR_A_HL_mem(pz80);
}
/* Logical OR A against A */
void
i_OR_A(z80 * pz80){
OR_A_n(pz80, REGA);
}
/* Compare B against A */
void
i_CP_B(z80 * pz80){
CP_A_n(pz80, REGB);
}
/* Compare C against A */
void
i_CP_C(z80 * pz80){
CP_A_n(pz80, REGC);
}
/* Compare D against A */
void
i_CP_D(z80 * pz80){
CP_A_n(pz80, REGD);
}
/* Compare E against A */
void
i_CP_E(z80 * pz80){
CP_A_n(pz80, REGE);
}
/* Compare H against A */
void
i_CP_H(z80 * pz80){
CP_A_n(pz80, REGH);
}
/* Compare L against A */
void
i_CP_L(z80 * pz80){
CP_A_n(pz80, REGL);
}
/* Compare value pointed by HL against A */
void
i_CP__HL_(z80 * pz80){
CP_A_HL_mem(pz80);
}
/* Compare A against A */
void
i_CP_A(z80 * pz80){
CP_A_n(pz80, REGA);
}
/* Return if last result was not zero */
void
i_RET_NZ(z80 * pz80){
RET_NZ(pz80);
}
/* Pop 16-bit value from stack into BC */
void
i_POP_BC(z80 * pz80){
POP_nn(pz80, REGB, REGC);
}
/* Absolute jump to 16-bit location if last result was not zero */
void
i_JP_NZ_nn(z80 * pz80){
JP_NZ_nn(pz80);
}
/* Absolute jump to 16-bit location */
void
i_JP_nn(z80 * pz80){
JP_nn(pz80);
}
/* Call routine at 16-bit location if last result was not zero */
void
i_CALL_NZ_nn(z80 * pz80){
CALL_NZ_nn(pz80);
}
/* Push 16-bit BC onto stack */
void
i_PUSH_BC(z80 * pz80){
PUSH_nn(pz80, REGB, REGC);
}
/* Add 8-bit immediate to A */
void
i_ADD_A_n(z80 * pz80){
ADD_A_immediate(pz80);
}
/* Call routine at address 0000h */
void
i_RST_0(z80 * pz80){
RST_n(pz80, 0);
}
/* Return if last result was zero */
void
i_RET_Z(z80 * pz80){
RET_Z(pz80);
}
/* Return to calling routine */
void
i_RET(z80 * pz80){
RET(pz80);
}
/* Absolute jump to 16-bit location if last result was zero */
void
i_JP_Z_nn(z80 * pz80){
JP_Z_nn(pz80);
}
/* Extended operations (two-byte instruction code) */
void
i_Ext_ops(z80 * pz80){
//printf("i_Ext_ops\n");
incPC(pz80, 1);
uint8_t op = rb(&(pz80->mmu), pz80->registers16[PC]);
#ifdef DEBUG
	printf("PC: %d, Opcode2: 0x%x\n", pz80->registers16[PC], op);
#endif
dispatchInstruction(pz80, op, 1);
}
/* Call routine at 16-bit location if last result was zero */
void
i_CALL_Z_nn(z80 * pz80){
CALL_Z_nn(pz80);
}
/* Call routine at 16-bit location */
void
i_CALL_nn(z80 * pz80){
CALL_nn(pz80);
}
/* Add 8-bit immediate and carry to A */
void
i_ADC_A_n(z80 * pz80){
ADC_A_immediate(pz80);
}
/* Call routine at address 0008h */
void
i_RST_8(z80 * pz80){
RST_n(pz80, 0x8);
}
/* Return if last result caused no carry */
void
i_RET_NC(z80 * pz80){
RET_NC(pz80);
}
/* Pop 16-bit value from stack into DE */
void
i_POP_DE(z80 * pz80){
POP_nn(pz80, REGD, REGE);
}
/* Absolute jump to 16-bit location if last result caused no carry */
void
i_JP_NC_nn(z80 * pz80){
JP_NC_nn(pz80);
}
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80){
//}
/* Call routine at 16-bit location if last result caused no carry */
void
i_CALL_NC_nn(z80 * pz80){
CALL_NC_nn(pz80);
}
/* Push 16-bit DE onto stack */
void
i_PUSH_DE(z80 * pz80){
PUSH_nn(pz80, REGD, REGE);
}
/* Subtract 8-bit immediate from A */
void
i_SUB_A_n(z80 * pz80){
SUB_A_immediate(pz80);
}
/* Call routine at address 0010h */
void
i_RST_10(z80 * pz80){
RST_n(pz80, 0x10);
}
/* Return if last result caused carry */
void
i_RET_C(z80 * pz80){
RET_C(pz80);
}
/* Enable interrupts and return to calling routine */
void
i_RETI(z80 * pz80){
RETI(pz80);
}
/* Absolute jump to 16-bit location if last result caused carry */
void
i_JP_C_nn(z80 * pz80){
JP_C_nn(pz80);
}
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80){
//}
/* Call routine at 16-bit location if last result caused carry */
void
i_CALL_C_nn(z80 * pz80){
CALL_C_nn(pz80);
}
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80){
//}
/* Subtract 8-bit immediate and carry from A */
void
i_SBC_A_n(z80 * pz80){
SBC_A_immediate(pz80);
}
/* Call routine at address 0018h */
void
i_RST_18(z80 * pz80){
RST_n(pz80, 0x18);
}
/* Save A at address pointed to by (FF00h + 8-bit immediate) */
void
i_LDH__n__A(z80 * pz80){
LDH_immediate_A(pz80);
}
/* Pop 16-bit value from stack into HL */
void
i_POP_HL(z80 * pz80){
POP_nn(pz80, REGH, REGL);
}
/* Save A at address pointed to by (FF00h + C) */
void
i_LDH__C__A(z80 * pz80){
LD_C_mem_A(pz80);
}
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80){
//}
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80){
//}
/* Push 16-bit HL onto stack */
void
i_PUSH_HL(z80 * pz80){
PUSH_nn(pz80, REGH, REGL);
}
/* Logical AND 8-bit immediate against A */
void
i_AND_n(z80 * pz80){
AND_A_immediate(pz80);
}
/* Call routine at address 0020h */
void
i_RST_20(z80 * pz80){
RST_n(pz80, 0x20);
}
/* Add signed 8-bit immediate to SP */
void
i_ADD_SP_d(z80 * pz80){
ADD_SP_d(pz80);
}
/* Jump to 16-bit value pointed by HL */
void
i_JP__HL_(z80 * pz80){
JP_HL(pz80);
}
/* Save A at given 16-bit address */
void
i_LD__nn__A(z80 * pz80){
LD_immediate_mem_n(pz80, REGA);
}
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80){
//}
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80){
//}
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80){
//}
/* Logical XOR 8-bit immediate against A */
void
i_XOR_n(z80 * pz80){
XOR_A_immediate(pz80);
}
/* Call routine at address 0028h */
void
i_RST_28(z80 * pz80){
RST_n(pz80, 0x28);
}
/* Load A from address pointed to by (FF00h + 8-bit immediate) */
void
i_LDH_A__n_(z80 * pz80){
LDH_A_immediate(pz80);
}
/* Pop 16-bit value from stack into AF */
void
i_POP_AF(z80 * pz80){
POP_nn(pz80, REGA, REGF);
}
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80){
//}
/* DIsable interrupts */
void
i_DI(z80 * pz80){
pz80->ime = 0; // TODO: in real GB, interrupts are disabled after next instruction.
pz80->tcycles = 4;
incPC(pz80, 1);
}
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80){
//}
/* Push 16-bit AF onto stack */
void
i_PUSH_AF(z80 * pz80){
PUSH_nn(pz80, REGA, REGF);
}
/* Logical OR 8-bit immediate against A */
void
i_OR_n(z80 * pz80){
OR_A_immediate(pz80);
}
/* Call routine at address 0030h */
void
i_RST_30(z80 * pz80){
RST_n(pz80, 0x30);
}
/* Add signed 8-bit immediate to SP and save result in HL */
void
i_LDHL_SP_d(z80 * pz80){
LDHL_SP_immediate(pz80);
}
/* Copy HL to SP */
void
i_LD_SP_HL(z80 * pz80){
LD_SP_HL(pz80);
}
/* Load A from given 16-bit address */
void
i_LD_A__nn_(z80 * pz80){
LD_n_immediate_mem(pz80, REGA);
}
/* Enable interrupts */
void
i_EI(z80 * pz80){
pz80->ime = 1; // TODO: real GB interrupts are enabled after next instruction
pz80->tcycles = 4;
incPC(pz80, 1);
}
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80){
//}
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80){
//}
/* Compare 8-bit immediate against A */
void
i_CP_n(z80 * pz80){
CP_A_immediate(pz80);
}
/* Call routine at address 0038h */
void
i_RST_38(z80 * pz80){
RST_n(pz80, 0x38);
}
/* Rotate B left with carry */
void
i2_RLC_B(z80 * pz80){
	RLC_n(pz80,REGB);
}
/* Rotate C left with carry */
void
i2_RLC_C(z80 * pz80){
	RLC_n(pz80,REGC);
}
/* Rotate D left with carry */
void
i2_RLC_D(z80 * pz80){
	RLC_n(pz80,REGD);
}
/* Rotate E left with carry */
void
i2_RLC_E(z80 * pz80){
	RLC_n(pz80,REGE);
}
/* Rotate H left with carry */
void
i2_RLC_H(z80 * pz80){
	RLC_n(pz80,REGH);
}
/* Rotate L left with carry */
void
i2_RLC_L(z80 * pz80){
	RLC_n(pz80,REGL);
}
/* Rotate value pointed by HL left with carry */
void
i2_RLC__HL_(z80 * pz80){
	RLC_HL(pz80);
}
/* Rotate A left with carry */
void
i2_RLC_A(z80 * pz80){
	RLC_n(pz80,REGA);
}
/* Rotate B right with carry */
void
i2_RRC_B(z80 * pz80){
	RRC_n(pz80,REGB);
}
/* Rotate C right with carry */
void
i2_RRC_C(z80 * pz80){
	RRC_n(pz80,REGC);
}
/* Rotate D right with carry */
void
i2_RRC_D(z80 * pz80){
	RRC_n(pz80,REGD);
}
/* Rotate E right with carry */
void
i2_RRC_E(z80 * pz80){
	RRC_n(pz80,REGE);
}
/* Rotate H right with carry */
void
i2_RRC_H(z80 * pz80){
	RRC_n(pz80,REGH);
}
/* Rotate L right with carry */
void
i2_RRC_L(z80 * pz80){
	RRC_n(pz80,REGL);
}
/* Rotate value pointed by HL right with carry */
void
i2_RRC__HL_(z80 * pz80){
	RRC_HL(pz80);
}
/* Rotate A right with carry */
void
i2_RRC_A(z80 * pz80){
	RRC_n(pz80,REGA);
}
/* Rotate B left */
void
i2_RL_B(z80 * pz80){
	RL_n(pz80,REGB);
}
/* Rotate C left */
void
i2_RL_C(z80 * pz80){
	RL_n(pz80,REGC);
}
/* Rotate D left */
void
i2_RL_D(z80 * pz80){
	RL_n(pz80,REGD);
}
/* Rotate E left */
void
i2_RL_E(z80 * pz80){
	RL_n(pz80,REGE);
}
/* Rotate H left */
void
i2_RL_H(z80 * pz80){
	RL_n(pz80,REGH);
}
/* Rotate L left */
void
i2_RL_L(z80 * pz80){
	RL_n(pz80,REGL);
}
/* Rotate value pointed by HL left */
void
i2_RL__HL_(z80 * pz80){
	RL_HL(pz80);
}
/* Rotate A left */
void
i2_RL_A(z80 * pz80){
	RL_n(pz80,REGA);
}
/* Rotate B right */
void
i2_RR_B(z80 * pz80){
	RR_n(pz80,REGB);
}
/* Rotate C right */
void
i2_RR_C(z80 * pz80){
	RR_n(pz80,REGC);
}
/* Rotate D right */
void
i2_RR_D(z80 * pz80){
	RR_n(pz80,REGD);
}
/* Rotate E right */
void
i2_RR_E(z80 * pz80){
	RR_n(pz80,REGE);
}
/* Rotate H right */
void
i2_RR_H(z80 * pz80){
	RR_n(pz80,REGH);
}
/* Rotate L right */
void
i2_RR_L(z80 * pz80){
	RR_n(pz80,REGL);
}
/* Rotate value pointed by HL right */
void
i2_RR__HL_(z80 * pz80){
	RR_HL(pz80);
}
/* Rotate A right */
void
i2_RR_A(z80 * pz80){
	RR_n(pz80,REGA);
}
/* Shift B left preserving sign */
void
i2_SLA_B(z80 * pz80){
	SLA_n(pz80,REGB);
}
/* Shift C left preserving sign */
void
i2_SLA_C(z80 * pz80){
	SLA_n(pz80,REGC);
}
/* Shift D left preserving sign */
void
i2_SLA_D(z80 * pz80){
	SLA_n(pz80,REGD);
}
/* Shift E left preserving sign */
void
i2_SLA_E(z80 * pz80){
	SLA_n(pz80,REGE);
}
/* Shift H left preserving sign */
void
i2_SLA_H(z80 * pz80){
	SLA_n(pz80,REGH);
}
/* Shift L left preserving sign */
void
i2_SLA_L(z80 * pz80){
	SLA_n(pz80,REGL);
}
/* Shift value pointed by HL left preserving sign */
void
i2_SLA__HL_(z80 * pz80){
	SLA_HL(pz80);
}
/* Shift A left preserving sign */
void
i2_SLA_A(z80 * pz80){
	SLA_n(pz80,REGA);
}
/* Shift B right preserving sign */
void
i2_SRA_B(z80 * pz80){
	SRA_n(pz80,REGB);
}
/* Shift C right preserving sign */
void
i2_SRA_C(z80 * pz80){
	SRA_n(pz80,REGC);
}
/* Shift D right preserving sign */
void
i2_SRA_D(z80 * pz80){
	SRA_n(pz80,REGD);
}
/* Shift E right preserving sign */
void
i2_SRA_E(z80 * pz80){
	SRA_n(pz80,REGE);
}
/* Shift H right preserving sign */
void
i2_SRA_H(z80 * pz80){
	SRA_n(pz80,REGH);
}
/* Shift L right preserving sign */
void
i2_SRA_L(z80 * pz80){
	SRA_n(pz80,REGL);
}
/* Shift value pointed by HL right preserving sign */
void
i2_SRA__HL_(z80 * pz80){
	SRA_HL(pz80);
}
/* Shift A right preserving sign */
void
i2_SRA_A(z80 * pz80){
	SRA_n(pz80,REGA);
}
/* Swap nybbles in B */
void
i2_SWAP_B(z80 * pz80){
	nibbleSwap(pz80,REGB);
}
/* Swap nybbles in C */
void
i2_SWAP_C(z80 * pz80){
	nibbleSwap(pz80,REGC);
}
/* Swap nybbles in D */
void
i2_SWAP_D(z80 * pz80){
	nibbleSwap(pz80,REGD);
}
/* Swap nybbles in E */
void
i2_SWAP_E(z80 * pz80){
	nibbleSwap(pz80,REGE);
}
/* Swap nybbles in H */
void
i2_SWAP_H(z80 * pz80){
	nibbleSwap(pz80,REGH);
}
/* Swap nybbles in L */
void
i2_SWAP_L(z80 * pz80){
	nibbleSwap(pz80,REGL);
}
/* Swap nybbles in value pointed by HL */
void
i2_SWAP__HL_(z80 * pz80){
	nibbleHLSwap(pz80);
}
/* Swap nybbles in A */
void
i2_SWAP_A(z80 * pz80){
	nibbleSwap(pz80,REGA);
}
/* Shift B right */
void
i2_SRL_B(z80 * pz80){
	SRL_n(pz80,REGB);
}
/* Shift C right */
void
i2_SRL_C(z80 * pz80){
	SRL_n(pz80,REGC);
}
/* Shift D right */
void
i2_SRL_D(z80 * pz80){
	SRL_n(pz80,REGD);
}
/* Shift E right */
void
i2_SRL_E(z80 * pz80){
	SRL_n(pz80,REGE);
}
/* Shift H right */
void
i2_SRL_H(z80 * pz80){
	SRL_n(pz80,REGH);
}
/* Shift L right */
void
i2_SRL_L(z80 * pz80){
	SRL_n(pz80,REGL);
}
/* Shift value pointed by HL right */
void
i2_SRL__HL_(z80 * pz80){
	SRL_HL(pz80);
}
/* Shift A right */
void
i2_SRL_A(z80 * pz80){
	SRL_n(pz80,REGA);
}
/* Test bit 0 of B */
void
i2_BIT_0_B(z80 * pz80){
	Test_Bit(pz80,REGB,0);
}
/* Test bit 0 of C */
void
i2_BIT_0_C(z80 * pz80){
	Test_Bit(pz80,REGC,0);
}
/* Test bit 0 of D */
void
i2_BIT_0_D(z80 * pz80){
	Test_Bit(pz80,REGD,0);
}
/* Test bit 0 of E */
void
i2_BIT_0_E(z80 * pz80){
	Test_Bit(pz80,REGE,0);
}
/* Test bit 0 of H */
void
i2_BIT_0_H(z80 * pz80){
	Test_Bit(pz80,REGH,0);
}
/* Test bit 0 of L */
void
i2_BIT_0_L(z80 * pz80){
	Test_Bit(pz80,REGL,0);
}
/* Test bit 0 of value pointed by HL */
void
i2_BIT_0__HL_(z80 * pz80){
	Test_HLBit(pz80,0);
}
/* Test bit 0 of A */
void
i2_BIT_0_A(z80 * pz80){
	Test_Bit(pz80,REGA,0);
}
/* Test bit 1 of B */
void
i2_BIT_1_B(z80 * pz80){
	Test_Bit(pz80,REGB,1);
}
/* Test bit 1 of C */
void
i2_BIT_1_C(z80 * pz80){
	Test_Bit(pz80,REGC,1);
}
/* Test bit 1 of D */
void
i2_BIT_1_D(z80 * pz80){
	Test_Bit(pz80,REGD,1);
}
/* Test bit 1 of E */
void
i2_BIT_1_E(z80 * pz80){
	Test_Bit(pz80,REGE,1);
}
/* Test bit 1 of H */
void
i2_BIT_1_H(z80 * pz80){
	Test_Bit(pz80,REGH,1);
}
/* Test bit 1 of L */
void
i2_BIT_1_L(z80 * pz80){
	Test_Bit(pz80,REGL,1);
}
/* Test bit 1 of value pointed by HL */
void
i2_BIT_1__HL_(z80 * pz80){
	Test_HLBit(pz80,1);
}
/* Test bit 1 of A */
void
i2_BIT_1_A(z80 * pz80){
	Test_Bit(pz80,REGA,1);
}
/* Test bit 2 of B */
void
i2_BIT_2_B(z80 * pz80){
	Test_Bit(pz80,REGB,2);
}
/* Test bit 2 of C */
void
i2_BIT_2_C(z80 * pz80){
	Test_Bit(pz80,REGC,2);
}
/* Test bit 2 of D */
void
i2_BIT_2_D(z80 * pz80){
	Test_Bit(pz80,REGD,2);
}
/* Test bit 2 of E */
void
i2_BIT_2_E(z80 * pz80){
	Test_Bit(pz80,REGE,2);
}
/* Test bit 2 of H */
void
i2_BIT_2_H(z80 * pz80){
	Test_Bit(pz80,REGH,2);
}
/* Test bit 2 of L */
void
i2_BIT_2_L(z80 * pz80){
	Test_Bit(pz80,REGL,2);
}
/* Test bit 2 of value pointed by HL */
void
i2_BIT_2__HL_(z80 * pz80){
	Test_HLBit(pz80,2);
}
/* Test bit 2 of A */
void
i2_BIT_2_A(z80 * pz80){
	Test_Bit(pz80,REGA,2);
}
/* Test bit 3 of B */
void
i2_BIT_3_B(z80 * pz80){
	Test_Bit(pz80,REGB,3);
}
/* Test bit 3 of C */
void
i2_BIT_3_C(z80 * pz80){
	Test_Bit(pz80,REGC,3);
}
/* Test bit 3 of D */
void
i2_BIT_3_D(z80 * pz80){
	Test_Bit(pz80,REGD,3);
}
/* Test bit 3 of E */
void
i2_BIT_3_E(z80 * pz80){
	Test_Bit(pz80,REGE,3);
}
/* Test bit 3 of H */
void
i2_BIT_3_H(z80 * pz80){
	Test_Bit(pz80,REGH,3);
}
/* Test bit 3 of L */
void
i2_BIT_3_L(z80 * pz80){
	Test_Bit(pz80,REGL,3);
}
/* Test bit 3 of value pointed by HL */
void
i2_BIT_3__HL_(z80 * pz80){
	Test_HLBit(pz80,3);
}
/* Test bit 3 of A */
void
i2_BIT_3_A(z80 * pz80){
	Test_Bit(pz80,REGA,3);
}
/* Test bit 4 of B */
void
i2_BIT_4_B(z80 * pz80){
	Test_Bit(pz80,REGB,4);
}
/* Test bit 4 of C */
void
i2_BIT_4_C(z80 * pz80){
	Test_Bit(pz80,REGC,4);
}
/* Test bit 4 of D */
void
i2_BIT_4_D(z80 * pz80){
	Test_Bit(pz80,REGD,4);
}
/* Test bit 4 of E */
void
i2_BIT_4_E(z80 * pz80){
	Test_Bit(pz80,REGE,4);
}
/* Test bit 4 of H */
void
i2_BIT_4_H(z80 * pz80){
	Test_Bit(pz80,REGH,4);
}
/* Test bit 4 of L */
void
i2_BIT_4_L(z80 * pz80){
	Test_Bit(pz80,REGL,4);
}
/* Test bit 4 of value pointed by HL */
void
i2_BIT_4__HL_(z80 * pz80){
	Test_HLBit(pz80,4);
}
/* Test bit 4 of A */
void
i2_BIT_4_A(z80 * pz80){
	Test_Bit(pz80,REGA,4);
}
/* Test bit 5 of B */
void
i2_BIT_5_B(z80 * pz80){
	Test_Bit(pz80,REGB,5);
}
/* Test bit 5 of C */
void
i2_BIT_5_C(z80 * pz80){
	Test_Bit(pz80,REGC,5);
}
/* Test bit 5 of D */
void
i2_BIT_5_D(z80 * pz80){
	Test_Bit(pz80,REGD,5);
}
/* Test bit 5 of E */
void
i2_BIT_5_E(z80 * pz80){
	Test_Bit(pz80,REGE,5);
}
/* Test bit 5 of H */
void
i2_BIT_5_H(z80 * pz80){
	Test_Bit(pz80,REGH,5);
}
/* Test bit 5 of L */
void
i2_BIT_5_L(z80 * pz80){
	Test_Bit(pz80,REGL,5);
}
/* Test bit 5 of value pointed by HL */
void
i2_BIT_5__HL_(z80 * pz80){
	Test_HLBit(pz80,5);
}
/* Test bit 5 of A */
void
i2_BIT_5_A(z80 * pz80){
	Test_Bit(pz80,REGA,5);
}
/* Test bit 6 of B */
void
i2_BIT_6_B(z80 * pz80){
	Test_Bit(pz80,REGB,6);
}
/* Test bit 6 of C */
void
i2_BIT_6_C(z80 * pz80){
	Test_Bit(pz80,REGC,6);
}
/* Test bit 6 of D */
void
i2_BIT_6_D(z80 * pz80){
	Test_Bit(pz80,REGD,6);
}
/* Test bit 6 of E */
void
i2_BIT_6_E(z80 * pz80){
	Test_Bit(pz80,REGE,6);
}
/* Test bit 6 of H */
void
i2_BIT_6_H(z80 * pz80){
	Test_Bit(pz80,REGH,6);
}
/* Test bit 6 of L */
void
i2_BIT_6_L(z80 * pz80){
	Test_Bit(pz80,REGL,6);
}
/* Test bit 6 of value pointed by HL */
void
i2_BIT_6__HL_(z80 * pz80){
	Test_HLBit(pz80,6);
}
/* Test bit 6 of A */
void
i2_BIT_6_A(z80 * pz80){
	Test_Bit(pz80,REGA,6);
}
/* Test bit 7 of B */
void
i2_BIT_7_B(z80 * pz80){
	Test_Bit(pz80,REGB,7);
}
/* Test bit 7 of C */
void
i2_BIT_7_C(z80 * pz80){
	Test_Bit(pz80,REGC,7);
}
/* Test bit 7 of D */
void
i2_BIT_7_D(z80 * pz80){
	Test_Bit(pz80,REGD,7);
}
/* Test bit 7 of E */
void
i2_BIT_7_E(z80 * pz80){
	Test_Bit(pz80,REGE,7);
}
/* Test bit 7 of H */
void
i2_BIT_7_H(z80 * pz80){
	Test_Bit(pz80,REGH,7);
}
/* Test bit 7 of L */
void
i2_BIT_7_L(z80 * pz80){
	Test_Bit(pz80,REGL,7);
}
/* Test bit 7 of value pointed by HL */
void
i2_BIT_7__HL_(z80 * pz80){
	Test_HLBit(pz80,7);
}
/* Test bit 7 of A */
void
i2_BIT_7_A(z80 * pz80){
	Test_Bit(pz80,REGA,7);
}
/* Clear (reset) bit 0 of B */
void
i2_RES_0_B(z80 * pz80){
	Reset_Bit(pz80,REGB,0);
}
/* Clear (reset) bit 0 of C */
void
i2_RES_0_C(z80 * pz80){
	Reset_Bit(pz80,REGC,0);
}
/* Clear (reset) bit 0 of D */
void
i2_RES_0_D(z80 * pz80){
	Reset_Bit(pz80,REGD,0);
}
/* Clear (reset) bit 0 of E */
void
i2_RES_0_E(z80 * pz80){
	Reset_Bit(pz80,REGE,0);
}
/* Clear (reset) bit 0 of H */
void
i2_RES_0_H(z80 * pz80){
	Reset_Bit(pz80,REGH,0);
}
/* Clear (reset) bit 0 of L */
void
i2_RES_0_L(z80 * pz80){
	Reset_Bit(pz80,REGL,0);
}
/* Clear (reset) bit 0 of value pointed by HL */
void
i2_RES_0__HL_(z80 * pz80){
	Reset_HLBit(pz80,0);
}
/* Clear (reset) bit 0 of A */
void
i2_RES_0_A(z80 * pz80){
	Reset_Bit(pz80,REGA,0);
}
/* Clear (reset) bit 1 of B */
void
i2_RES_1_B(z80 * pz80){
	Reset_Bit(pz80,REGB,1);
}
/* Clear (reset) bit 1 of C */
void
i2_RES_1_C(z80 * pz80){
	Reset_Bit(pz80,REGC,1);
}
/* Clear (reset) bit 1 of D */
void
i2_RES_1_D(z80 * pz80){
	Reset_Bit(pz80,REGD,1);
}
/* Clear (reset) bit 1 of E */
void
i2_RES_1_E(z80 * pz80){
	Reset_Bit(pz80,REGE,1);
}
/* Clear (reset) bit 1 of H */
void
i2_RES_1_H(z80 * pz80){
	Reset_Bit(pz80,REGH,1);
}
/* Clear (reset) bit 1 of L */
void
i2_RES_1_L(z80 * pz80){
	Reset_Bit(pz80,REGL,1);
}
/* Clear (reset) bit 1 of value pointed by HL */
void
i2_RES_1__HL_(z80 * pz80){
	Reset_HLBit(pz80,1);
}
/* Clear (reset) bit 1 of A */
void
i2_RES_1_A(z80 * pz80){
	Reset_Bit(pz80,REGA,1);
}
/* Clear (reset) bit 2 of B */
void
i2_RES_2_B(z80 * pz80){
	Reset_Bit(pz80,REGB,2);
}
/* Clear (reset) bit 2 of C */
void
i2_RES_2_C(z80 * pz80){
	Reset_Bit(pz80,REGC,2);
}
/* Clear (reset) bit 2 of D */
void
i2_RES_2_D(z80 * pz80){
	Reset_Bit(pz80,REGD,2);
}
/* Clear (reset) bit 2 of E */
void
i2_RES_2_E(z80 * pz80){
	Reset_Bit(pz80,REGE,2);
}
/* Clear (reset) bit 2 of H */
void
i2_RES_2_H(z80 * pz80){
	Reset_Bit(pz80,REGH,2);
}
/* Clear (reset) bit 2 of L */
void
i2_RES_2_L(z80 * pz80){
	Reset_Bit(pz80,REGL,2);
}
/* Clear (reset) bit 2 of value pointed by HL */
void
i2_RES_2__HL_(z80 * pz80){
	Reset_HLBit(pz80,2);
}
/* Clear (reset) bit 2 of A */
void
i2_RES_2_A(z80 * pz80){
	Reset_Bit(pz80,REGA,2);
}
/* Clear (reset) bit 3 of B */
void
i2_RES_3_B(z80 * pz80){
	Reset_Bit(pz80,REGB,3);
}
/* Clear (reset) bit 3 of C */
void
i2_RES_3_C(z80 * pz80){
	Reset_Bit(pz80,REGC,3);
}
/* Clear (reset) bit 3 of D */
void
i2_RES_3_D(z80 * pz80){
	Reset_Bit(pz80,REGD,3);
}
/* Clear (reset) bit 3 of E */
void
i2_RES_3_E(z80 * pz80){
	Reset_Bit(pz80,REGE,3);
}
/* Clear (reset) bit 3 of H */
void
i2_RES_3_H(z80 * pz80){
	Reset_Bit(pz80,REGH,3);
}
/* Clear (reset) bit 3 of L */
void
i2_RES_3_L(z80 * pz80){
	Reset_Bit(pz80,REGA,3);
}
/* Clear (reset) bit 3 of value pointed by HL */
void
i2_RES_3__HL_(z80 * pz80){
	Reset_HLBit(pz80,3);
}
/* Clear (reset) bit 3 of A */
void
i2_RES_3_A(z80 * pz80){
	Reset_Bit(pz80,REGA,3);
}
/* Clear (reset) bit 4 of B */
void
i2_RES_4_B(z80 * pz80){
	Reset_Bit(pz80,REGB,4);
}
/* Clear (reset) bit 4 of C */
void
i2_RES_4_C(z80 * pz80){
	Reset_Bit(pz80,REGC,4);
}
/* Clear (reset) bit 4 of D */
void
i2_RES_4_D(z80 * pz80){
	Reset_Bit(pz80,REGD,4);
}
/* Clear (reset) bit 4 of E */
void
i2_RES_4_E(z80 * pz80){
	Reset_Bit(pz80,REGE,4);
}
/* Clear (reset) bit 4 of H */
void
i2_RES_4_H(z80 * pz80){
	Reset_Bit(pz80,REGH,4);
}
/* Clear (reset) bit 4 of L */
void
i2_RES_4_L(z80 * pz80){
	Reset_Bit(pz80,REGL,4);
}
/* Clear (reset) bit 4 of value pointed by HL */
void
i2_RES_4__HL_(z80 * pz80){
	Reset_HLBit(pz80,4);
}
/* Clear (reset) bit 4 of A */
void
i2_RES_4_A(z80 * pz80){
	Reset_Bit(pz80,REGA,4);
}
/* Clear (reset) bit 5 of B */
void
i2_RES_5_B(z80 * pz80){
	Reset_Bit(pz80,REGB,5);
}
/* Clear (reset) bit 5 of C */
void
i2_RES_5_C(z80 * pz80){
	Reset_Bit(pz80,REGC,5);
}
/* Clear (reset) bit 5 of D */
void
i2_RES_5_D(z80 * pz80){
	Reset_Bit(pz80,REGD,5);
}
/* Clear (reset) bit 5 of E */
void
i2_RES_5_E(z80 * pz80){
	Reset_Bit(pz80,REGE,5);
}
/* Clear (reset) bit 5 of H */
void
i2_RES_5_H(z80 * pz80){
	Reset_Bit(pz80,REGH,5);
}
/* Clear (reset) bit 5 of L */
void
i2_RES_5_L(z80 * pz80){
	Reset_Bit(pz80,REGL,5);
}
/* Clear (reset) bit 5 of value pointed by HL */
void
i2_RES_5__HL_(z80 * pz80){
	Reset_HLBit(pz80,5);
}
/* Clear (reset) bit 5 of A */
void
i2_RES_5_A(z80 * pz80){
	Reset_Bit(pz80,REGA,5);
}
/* Clear (reset) bit 6 of B */
void
i2_RES_6_B(z80 * pz80){
	Reset_Bit(pz80,REGB,6);
}
/* Clear (reset) bit 6 of C */
void
i2_RES_6_C(z80 * pz80){
	Reset_Bit(pz80,REGC,6);
}
/* Clear (reset) bit 6 of D */
void
i2_RES_6_D(z80 * pz80){
	Reset_Bit(pz80,REGD,6);
}
/* Clear (reset) bit 6 of E */
void
i2_RES_6_E(z80 * pz80){
	Reset_Bit(pz80,REGE,6);
}
/* Clear (reset) bit 6 of H */
void
i2_RES_6_H(z80 * pz80){
	Reset_Bit(pz80,REGH,6);
}
/* Clear (reset) bit 6 of L */
void
i2_RES_6_L(z80 * pz80){
	Reset_Bit(pz80,REGL,6);
}
/* Clear (reset) bit 6 of value pointed by HL */
void
i2_RES_6__HL_(z80 * pz80){
	Reset_HLBit(pz80,6);
}
/* Clear (reset) bit 6 of A */
void
i2_RES_6_A(z80 * pz80){
	Reset_Bit(pz80,REGA,6);
}
/* Clear (reset) bit 7 of B */
void
i2_RES_7_B(z80 * pz80){
	Reset_Bit(pz80,REGB,7);
}
/* Clear (reset) bit 7 of C */
void
i2_RES_7_C(z80 * pz80){
	Reset_Bit(pz80,REGC,7);
}
/* Clear (reset) bit 7 of D */
void
i2_RES_7_D(z80 * pz80){
	Reset_Bit(pz80,REGD,7);
}
/* Clear (reset) bit 7 of E */
void
i2_RES_7_E(z80 * pz80){
	Reset_Bit(pz80,REGE,7);
}
/* Clear (reset) bit 7 of H */
void
i2_RES_7_H(z80 * pz80){
	Reset_Bit(pz80,REGH,7);
}
/* Clear (reset) bit 7 of L */
void
i2_RES_7_L(z80 * pz80){
	Reset_Bit(pz80,REGL,7);
}
/* Clear (reset) bit 7 of value pointed by HL */
void
i2_RES_7__HL_(z80 * pz80){
	Reset_HLBit(pz80,7);
}
/* Clear (reset) bit 7 of A */
void
i2_RES_7_A(z80 * pz80){
	Reset_Bit(pz80,REGA,7);
}
/* Set bit 0 of B */
void
i2_SET_0_B(z80 * pz80){
	Set_Bit(pz80,REGB,0);
}
/* Set bit 0 of C */
void
i2_SET_0_C(z80 * pz80){
	Set_Bit(pz80,REGC,0);
}
/* Set bit 0 of D */
void
i2_SET_0_D(z80 * pz80){
	Set_Bit(pz80,REGD,0);
}
/* Set bit 0 of E */
void
i2_SET_0_E(z80 * pz80){
	Set_Bit(pz80,REGE,0);
}
/* Set bit 0 of H */
void
i2_SET_0_H(z80 * pz80){
	Set_Bit(pz80,REGH,0);
}
/* Set bit 0 of L */
void
i2_SET_0_L(z80 * pz80){
	Set_Bit(pz80,REGL,0);
}
/* Set bit 0 of value pointed by HL */
void
i2_SET_0__HL_(z80 * pz80){
	Set_HLBit(pz80,0);
}
/* Set bit 0 of A */
void
i2_SET_0_A(z80 * pz80){
	Set_Bit(pz80,REGA,0);
}
/* Set bit 1 of B */
void
i2_SET_1_B(z80 * pz80){
	Set_Bit(pz80,REGB,1);
}
/* Set bit 1 of C */
void
i2_SET_1_C(z80 * pz80){
	Set_Bit(pz80,REGC,1);
}
/* Set bit 1 of D */
void
i2_SET_1_D(z80 * pz80){
	Set_Bit(pz80,REGD,1);
}
/* Set bit 1 of E */
void
i2_SET_1_E(z80 * pz80){
	Set_Bit(pz80,REGE,1);
}
/* Set bit 1 of H */
void
i2_SET_1_H(z80 * pz80){
	Set_Bit(pz80,REGH,1);
}
/* Set bit 1 of L */
void
i2_SET_1_L(z80 * pz80){
	Set_Bit(pz80,REGL,1);
}
/* Set bit 1 of value pointed by HL */
void
i2_SET_1__HL_(z80 * pz80){
	Set_HLBit(pz80,1);
}
/* Set bit 1 of A */
void
i2_SET_1_A(z80 * pz80){
	Set_Bit(pz80,REGA,1);
}
/* Set bit 2 of B */
void
i2_SET_2_B(z80 * pz80){
	Set_Bit(pz80,REGB,2);
}
/* Set bit 2 of C */
void
i2_SET_2_C(z80 * pz80){
	Set_Bit(pz80,REGC,2);
}
/* Set bit 2 of D */
void
i2_SET_2_D(z80 * pz80){
	Set_Bit(pz80,REGD,2);
}
/* Set bit 2 of E */
void
i2_SET_2_E(z80 * pz80){
	Set_Bit(pz80,REGE,2);
}
/* Set bit 2 of H */
void
i2_SET_2_H(z80 * pz80){
	Set_Bit(pz80,REGH,2);
}
/* Set bit 2 of L */
void
i2_SET_2_L(z80 * pz80){
	Set_Bit(pz80,REGL,2);
}
/* Set bit 2 of value pointed by HL */
void
i2_SET_2__HL_(z80 * pz80){
	Set_HLBit(pz80,2);
}
/* Set bit 2 of A */
void
i2_SET_2_A(z80 * pz80){
	Set_Bit(pz80,REGA,2);
}
/* Set bit 3 of B */
void
i2_SET_3_B(z80 * pz80){
	Set_Bit(pz80,REGB,3);
}
/* Set bit 3 of C */
void
i2_SET_3_C(z80 * pz80){
	Set_Bit(pz80,REGC,3);
}
/* Set bit 3 of D */
void
i2_SET_3_D(z80 * pz80){
	Set_Bit(pz80,REGD,3);
}
/* Set bit 3 of E */
void
i2_SET_3_E(z80 * pz80){
	Set_Bit(pz80,REGE,3);
}
/* Set bit 3 of H */
void
i2_SET_3_H(z80 * pz80){
	Set_Bit(pz80,REGH,3);
}
/* Set bit 3 of L */
void
i2_SET_3_L(z80 * pz80){
	Set_Bit(pz80,REGL,3);
}
/* Set bit 3 of value pointed by HL */
void
i2_SET_3__HL_(z80 * pz80){
	Set_HLBit(pz80,3);
}
/* Set bit 3 of A */
void
i2_SET_3_A(z80 * pz80){
	Set_Bit(pz80,REGA,3);
}
/* Set bit 4 of B */
void
i2_SET_4_B(z80 * pz80){
	Set_Bit(pz80,REGB,4);
}
/* Set bit 4 of C */
void
i2_SET_4_C(z80 * pz80){
	Set_Bit(pz80,REGC,4);
}
/* Set bit 4 of D */
void
i2_SET_4_D(z80 * pz80){
	Set_Bit(pz80,REGD,4);
}
/* Set bit 4 of E */
void
i2_SET_4_E(z80 * pz80){
	Set_Bit(pz80,REGE,4);
}
/* Set bit 4 of H */
void
i2_SET_4_H(z80 * pz80){
	Set_Bit(pz80,REGH,4);
}
/* Set bit 4 of L */
void
i2_SET_4_L(z80 * pz80){
	Set_Bit(pz80,REGL,4);
}
/* Set bit 4 of value pointed by HL */
void
i2_SET_4__HL_(z80 * pz80){
	Set_HLBit(pz80,4);
}
/* Set bit 4 of A */
void
i2_SET_4_A(z80 * pz80){
	Set_Bit(pz80,REGA,4);
}
/* Set bit 5 of B */
void
i2_SET_5_B(z80 * pz80){
	Set_Bit(pz80,REGB,5);
}
/* Set bit 5 of C */
void
i2_SET_5_C(z80 * pz80){
	Set_Bit(pz80,REGC,5);
}
/* Set bit 5 of D */
void
i2_SET_5_D(z80 * pz80){
	Set_Bit(pz80,REGD,5);
}
/* Set bit 5 of E */
void
i2_SET_5_E(z80 * pz80){
	Set_Bit(pz80,REGE,5);
}
/* Set bit 5 of H */
void
i2_SET_5_H(z80 * pz80){
	Set_Bit(pz80,REGH,5);
}
/* Set bit 5 of L */
void
i2_SET_5_L(z80 * pz80){
	Set_Bit(pz80,REGL,5);
}
/* Set bit 5 of value pointed by HL */
void
i2_SET_5__HL_(z80 * pz80){
	Set_HLBit(pz80,5);
}
/* Set bit 5 of A */
void
i2_SET_5_A(z80 * pz80){
	Set_Bit(pz80,REGA,5);
}
/* Set bit 6 of B */
void
i2_SET_6_B(z80 * pz80){
	Set_Bit(pz80,REGB,6);
}
/* Set bit 6 of C */
void
i2_SET_6_C(z80 * pz80){
	Set_Bit(pz80,REGC,6);
}
/* Set bit 6 of D */
void
i2_SET_6_D(z80 * pz80){
	Set_Bit(pz80,REGD,6);
}
/* Set bit 6 of E */
void
i2_SET_6_E(z80 * pz80){
	Set_Bit(pz80,REGE,6);
}
/* Set bit 6 of H */
void
i2_SET_6_H(z80 * pz80){
	Set_Bit(pz80,REGH,6);
}
/* Set bit 6 of L */
void
i2_SET_6_L(z80 * pz80){
	Set_Bit(pz80,REGL,6);
}
/* Set bit 6 of value pointed by HL */
void
i2_SET_6__HL_(z80 * pz80){
	Set_HLBit(pz80,6);
}
/* Set bit 6 of A */
void
i2_SET_6_A(z80 * pz80){
	Set_Bit(pz80,REGA,6);
}
/* Set bit 7 of B */
void
i2_SET_7_B(z80 * pz80){
	Set_Bit(pz80,REGB,7);
}
/* Set bit 7 of C */
void
i2_SET_7_C(z80 * pz80){
	Set_Bit(pz80,REGC,7);
}
/* Set bit 7 of D */
void
i2_SET_7_D(z80 * pz80){
	Set_Bit(pz80,REGD,7);
}
/* Set bit 7 of E */
void
i2_SET_7_E(z80 * pz80){
	Set_Bit(pz80,REGE,7);
}
/* Set bit 7 of H */
void
i2_SET_7_H(z80 * pz80){
	Set_Bit(pz80,REGH,7);
}
/* Set bit 7 of L */
void
i2_SET_7_L(z80 * pz80){
	Set_Bit(pz80,REGL,7);
}
/* Set bit 7 of value pointed by HL */
void
i2_SET_7__HL_(z80 * pz80){
	Set_HLBit(pz80,7);
}
/* Set bit 7 of A */
void
i2_SET_7_A(z80 * pz80){
	Set_Bit(pz80,REGA,7);
}

/********************/

