#ifndef __z80__ 
#define __z80__
#include "stdint.h"
#include "mmu.h"

/* constants */


enum{
CARRY = 4,
HALFCARRY = 5,
SUB = 6,
ZERO = 7
};
enum {
REGA = 0,
REGB = 1,
REGC = 2,
REGD = 3,
REGE = 4,
REGF = 5,
REGH = 6,
REGL = 7,
REGMEM = 8
};

enum{
	PC = 0,
	SP = 1
	}; 

#define P0_P13_INT 4
#define SERIALINT 3
#define TOVF 2
#define LCDCINT 1
#define VBLANKINT 0



/*Data types*/

typedef struct _z80 {
	uint8_t registers[9]; //ensure this size matches up with the defines above
	uint16_t registers16[2];
	MMU mmu;
	uint8_t ime; // interrupt enable
	int tcycles;
	//int doSecondaryOpcode;
} z80;




/* functions defined as macros */
	
#define getInterruptEnabled(pz80, flag) ((rb(&(pz80)->mmu,0xffff) >> (flag)) & 0x1)
#define getInterruptPending(pz80, flag) ((rb(&(pz80)->mmu,0xff0f) >> (flag)) & 0x1)
#define setInterruptPending(pz80, flag) ((wb(&(pz80)->mmu,0xff0f,rb(&(pz80)->mmu,0xff0f)|1<<(flag))))

#define setFlag(X,FLAG) (X |= (1<<(FLAG)))
#define getFlag(X,FLAG) ((X >> (FLAG)) &1)


#define getRegister8(pz80,X) ((pz80)->registers[(X)])
#define getRegister16(pz80,H,L) ((((uint16_t)(pz80)->registers[(H)]) << 8) + (uint16_t)((pz80)->registers[(L)]))

#define setRegister8(pz80,X,val) ((pz80)->registers[(X)] = (val))
#define setRegister16(pz80,H,L,val) (pz80)->registers[(H)] = (((val) >> 8) & 255); (pz80)->registers[(L)] = ((val) & 255)

#define getImmediate(pz80) (rb(&((pz80)->mmu), (pz80)->registers16[PC] + 1))
#define getImmediate16(pz80) ((((uint16_t)rb(&((pz80)->mmu), (pz80)->registers16[PC] + 2)) << 8) + (rb(&((pz80)->mmu), (pz80)->registers16[PC] + 1)))

#define loadRegFromMemory8(pz80, reg, address) ((pz80)->registers[(reg)] = rb(&((pz80)->mmu), (address)))
#define loadRegFromMemory16(pz80, regH, regL, address) loadRegFromMemory8((pz80), (regL), (address)); loadRegFromMemory8((pz80), (regH), (address+1))

#define incPC(pz80, n) ((pz80)->registers16[PC] += (n))

#define Set_Bit(cpu,reg,bit) Set_BitToVal(cpu,reg,bit,1)
#define Set_HLBit(cpu,bit) Set_HLBitToVal(cpu,bit,1)
#define Reset_Bit(cpu,reg,bit) Set_BitToVal(cpu,reg,bit,0)
#define Reset_HLBit(cpu,bit) Set_HLBitToVal(cpu,bit,0)



/* general purpose functions */
	
extern
uint8_t 
buildStatusFlag(int zero, int sub, int halfcarry,int carry);

extern
void 
loadRegMemFromHL(z80*pz80);

extern
void 
saveRegMemToHL(z80*pz80);

void push(z80*pz80, uint8_t val);

void
triggerInterrupt(z80*pz80,int interrupt);

void 
executeNextInstruction(z80 * pz80);

void 
checkAndTriggerInterrupts(z80* pz80);

void 
dispatchInstruction(z80 * pz80,uint8_t opcode, int secondary);

void
initZ80(z80*pz80);



/* opcode functions here */
//////////////////////////////////////////////////////////////////////
/* No Operation */
void
i_NOP(z80 * pz80);
/* Load 16-bit immediate into BC */
void
i_LD_BC_nn(z80 * pz80);
/* Save A to address pointed by BC */
void
i_LD__BC__A(z80 * pz80);
/* Increment 16-bit BC */
void
i_INC_BC(z80 * pz80);
/* Increment B */
void
i_INC_B(z80 * pz80);
/* Decrement B */
void
i_DEC_B(z80 * pz80);
/* Load 8-bit immediate into B */
void
i_LD_B_n(z80 * pz80);
/* Rotate A left with carry */
void
i_RLC_A(z80 * pz80);
/* Save SP to given address */
void
i_LD__nn__SP(z80 * pz80);
/* Add 16-bit BC to HL */
void
i_ADD_HL_BC(z80 * pz80);
/* Load A from address pointed to by BC */
void
i_LD_A__BC_(z80 * pz80);
/* Decrement 16-bit BC */
void
i_DEC_BC(z80 * pz80);
/* Increment C */
void
i_INC_C(z80 * pz80);
/* Decrement C */
void
i_DEC_C(z80 * pz80);
/* Load 8-bit immediate into C */
void
i_LD_C_n(z80 * pz80);
/* Rotate A right with carry */
void
i_RRC_A(z80 * pz80);
/* Stop processor */
void
i_STOP(z80 * pz80);
/* Load 16-bit immediate into DE */
void
i_LD_DE_nn(z80 * pz80);
/* Save A to address pointed by DE */
void
i_LD__DE__A(z80 * pz80);
/* Increment 16-bit DE */
void
i_INC_DE(z80 * pz80);
/* Increment D */
void
i_INC_D(z80 * pz80);
/* Decrement D */
void
i_DEC_D(z80 * pz80);
/* Load 8-bit immediate into D */
void
i_LD_D_n(z80 * pz80);
/* Rotate A left */
void
i_RL_A(z80 * pz80);
/* Relative jump by signed immediate */
void
i_JR_n(z80 * pz80);
/* Add 16-bit DE to HL */
void
i_ADD_HL_DE(z80 * pz80);
/* Load A from address pointed to by DE */
void
i_LD_A__DE_(z80 * pz80);
/* Decrement 16-bit DE */
void
i_DEC_DE(z80 * pz80);
/* Increment E */
void
i_INC_E(z80 * pz80);
/* Decrement E */
void
i_DEC_E(z80 * pz80);
/* Load 8-bit immediate into E */
void
i_LD_E_n(z80 * pz80);
/* Rotate A right */
void
i_RR_A(z80 * pz80);
/* Relative jump by signed immediate if last result was not zero */
void
i_JR_NZ_n(z80 * pz80);
/* Load 16-bit immediate into HL */
void
i_LD_HL_nn(z80 * pz80);
/* Save A to address pointed by HL, and increment HL */
void
i_LDI__HL__A(z80 * pz80);
/* Increment 16-bit HL */
void
i_INC_HL(z80 * pz80);
/* Increment H */
void
i_INC_H(z80 * pz80);
/* Decrement H */
void
i_DEC_H(z80 * pz80);
/* Load 8-bit immediate into H */
void
i_LD_H_n(z80 * pz80);
/* Adjust A for BCD addition */
void
i_DAA(z80 * pz80);
/* Relative jump by signed immediate if last result was zero */
void
i_JR_Z_n(z80 * pz80);
/* Add 16-bit HL to HL */
void
i_ADD_HL_HL(z80 * pz80);
/* Load A from address pointed to by HL, and increment HL */
void
i_LDI_A__HL_(z80 * pz80);
/* Decrement 16-bit HL */
void
i_DEC_HL(z80 * pz80);
/* Increment L */
void
i_INC_L(z80 * pz80);
/* Decrement L */
void
i_DEC_L(z80 * pz80);
/* Load 8-bit immediate into L */
void
i_LD_L_n(z80 * pz80);
/* Complement (logical NOT) on A */
void
i_CPL(z80 * pz80);
/* Relative jump by signed immediate if last result caused no carry */
void
i_JR_NC_n(z80 * pz80);
/* Load 16-bit immediate into SP */
void
i_LD_SP_nn(z80 * pz80);
/* Save A to address pointed by HL, and decrement HL */
void
i_LDD__HL__A(z80 * pz80);
/* Increment 16-bit HL */
void
i_INC_SP(z80 * pz80);
/* Increment value pointed by HL */
void
i_INC__HL_(z80 * pz80);
/* Decrement value pointed by HL */
void
i_DEC__HL_(z80 * pz80);
/* Load 8-bit immediate into address pointed by HL */
void
i_LD__HL__n(z80 * pz80);
/* Set carry flag */
void
i_SCF(z80 * pz80);
/* Relative jump by signed immediate if last result caused carry */
void
i_JR_C_n(z80 * pz80);
/* Add 16-bit SP to HL */
void
i_ADD_HL_SP(z80 * pz80);
/* Load A from address pointed to by HL, and decrement HL */
void
i_LDD_A__HL_(z80 * pz80);
/* Decrement 16-bit SP */
void
i_DEC_SP(z80 * pz80);
/* Increment A */
void
i_INC_A(z80 * pz80);
/* Decrement A */
void
i_DEC_A(z80 * pz80);
/* Load 8-bit immediate into A */
void
i_LD_A_n(z80 * pz80);
/* Clear carry flag */
void
i_CCF(z80 * pz80);
/* Copy B to B */
void
i_LD_B_B(z80 * pz80);
/* Copy C to B */
void
i_LD_B_C(z80 * pz80);
/* Copy D to B */
void
i_LD_B_D(z80 * pz80);
/* Copy E to B */
void
i_LD_B_E(z80 * pz80);
/* Copy H to B */
void
i_LD_B_H(z80 * pz80);
/* Copy L to B */
void
i_LD_B_L(z80 * pz80);
/* Copy value pointed by HL to B */
void
i_LD_B__HL_(z80 * pz80);
/* Copy A to B */
void
i_LD_B_A(z80 * pz80);
/* Copy B to C */
void
i_LD_C_B(z80 * pz80);
/* Copy C to C */
void
i_LD_C_C(z80 * pz80);
/* Copy D to C */
void
i_LD_C_D(z80 * pz80);
/* Copy E to C */
void
i_LD_C_E(z80 * pz80);
/* Copy H to C */
void
i_LD_C_H(z80 * pz80);
/* Copy L to C */
void
i_LD_C_L(z80 * pz80);
/* Copy value pointed by HL to C */
void
i_LD_C__HL_(z80 * pz80);
/* Copy A to C */
void
i_LD_C_A(z80 * pz80);
/* Copy B to D */
void
i_LD_D_B(z80 * pz80);
/* Copy C to D */
void
i_LD_D_C(z80 * pz80);
/* Copy D to D */
void
i_LD_D_D(z80 * pz80);
/* Copy E to D */
void
i_LD_D_E(z80 * pz80);
/* Copy H to D */
void
i_LD_D_H(z80 * pz80);
/* Copy L to D */
void
i_LD_D_L(z80 * pz80);
/* Copy value pointed by HL to D */
void
i_LD_D__HL_(z80 * pz80);
/* Copy A to D */
void
i_LD_D_A(z80 * pz80);
/* Copy B to E */
void
i_LD_E_B(z80 * pz80);
/* Copy C to E */
void
i_LD_E_C(z80 * pz80);
/* Copy D to E */
void
i_LD_E_D(z80 * pz80);
/* Copy E to E */
void
i_LD_E_E(z80 * pz80);
/* Copy H to E */
void
i_LD_E_H(z80 * pz80);
/* Copy L to E */
void
i_LD_E_L(z80 * pz80);
/* Copy value pointed by HL to E */
void
i_LD_E__HL_(z80 * pz80);
/* Copy A to E */
void
i_LD_E_A(z80 * pz80);
/* Copy B to H */
void
i_LD_H_B(z80 * pz80);
/* Copy C to H */
void
i_LD_H_C(z80 * pz80);
/* Copy D to H */
void
i_LD_H_D(z80 * pz80);
/* Copy E to H */
void
i_LD_H_E(z80 * pz80);
/* Copy H to H */
void
i_LD_H_H(z80 * pz80);
/* Copy L to H */
void
i_LD_H_L(z80 * pz80);
/* Copy value pointed by HL to H */
void
i_LD_H__HL_(z80 * pz80);
/* Copy A to H */
void
i_LD_H_A(z80 * pz80);
/* Copy B to L */
void
i_LD_L_B(z80 * pz80);
/* Copy C to L */
void
i_LD_L_C(z80 * pz80);
/* Copy D to L */
void
i_LD_L_D(z80 * pz80);
/* Copy E to L */
void
i_LD_L_E(z80 * pz80);
/* Copy H to L */
void
i_LD_L_H(z80 * pz80);
/* Copy L to L */
void
i_LD_L_L(z80 * pz80);
/* Copy value pointed by HL to L */
void
i_LD_L__HL_(z80 * pz80);
/* Copy A to L */
void
i_LD_L_A(z80 * pz80);
/* Copy B to address pointed by HL */
void
i_LD__HL__B(z80 * pz80);
/* Copy C to address pointed by HL */
void
i_LD__HL__C(z80 * pz80);
/* Copy D to address pointed by HL */
void
i_LD__HL__D(z80 * pz80);
/* Copy E to address pointed by HL */
void
i_LD__HL__E(z80 * pz80);
/* Copy H to address pointed by HL */
void
i_LD__HL__H(z80 * pz80);
/* Copy L to address pointed by HL */
void
i_LD__HL__L(z80 * pz80);
/* Halt processor */
void
i_HALT(z80 * pz80);
/* Copy A to address pointed by HL */
void
i_LD__HL__A(z80 * pz80);
/* Copy B to A */
void
i_LD_A_B(z80 * pz80);
/* Copy C to A */
void
i_LD_A_C(z80 * pz80);
/* Copy D to A */
void
i_LD_A_D(z80 * pz80);
/* Copy E to A */
void
i_LD_A_E(z80 * pz80);
/* Copy H to A */
void
i_LD_A_H(z80 * pz80);
/* Copy L to A */
void
i_LD_A_L(z80 * pz80);
/* Copy value pointed by HL to A */
void
i_LD_A__HL_(z80 * pz80);
/* Copy A to A */
void
i_LD_A_A(z80 * pz80);
/* Add B to A */
void
i_ADD_A_B(z80 * pz80);
/* Add C to A */
void
i_ADD_A_C(z80 * pz80);
/* Add D to A */
void
i_ADD_A_D(z80 * pz80);
/* Add E to A */
void
i_ADD_A_E(z80 * pz80);
/* Add H to A */
void
i_ADD_A_H(z80 * pz80);
/* Add L to A */
void
i_ADD_A_L(z80 * pz80);
/* Add value pointed by HL to A */
void
i_ADD_A__HL_(z80 * pz80);
/* Add A to A */
void
i_ADD_A_A(z80 * pz80);
/* Add B and carry flag to A */
void
i_ADC_A_B(z80 * pz80);
/* Add C and carry flag to A */
void
i_ADC_A_C(z80 * pz80);
/* Add D and carry flag to A */
void
i_ADC_A_D(z80 * pz80);
/* Add E and carry flag to A */
void
i_ADC_A_E(z80 * pz80);
/* Add H and carry flag to A */
void
i_ADC_A_H(z80 * pz80);
/* Add and carry flag L to A */
void
i_ADC_A_L(z80 * pz80);
/* Add value pointed by HL and carry flag to A */
void
i_ADC_A__HL_(z80 * pz80);
/* Add A and carry flag to A */
void
i_ADC_A_A(z80 * pz80);
/* Subtract B from A */
void
i_SUB_A_B(z80 * pz80);
/* Subtract C from A */
void
i_SUB_A_C(z80 * pz80);
/* Subtract D from A */
void
i_SUB_A_D(z80 * pz80);
/* Subtract E from A */
void
i_SUB_A_E(z80 * pz80);
/* Subtract H from A */
void
i_SUB_A_H(z80 * pz80);
/* Subtract L from A */
void
i_SUB_A_L(z80 * pz80);
/* Subtract value pointed by HL from A */
void
i_SUB_A__HL_(z80 * pz80);
/* Subtract A from A */
void
i_SUB_A_A(z80 * pz80);
/* Subtract B and carry flag from A */
void
i_SBC_A_B(z80 * pz80);
/* Subtract C and carry flag from A */
void
i_SBC_A_C(z80 * pz80);
/* Subtract D and carry flag from A */
void
i_SBC_A_D(z80 * pz80);
/* Subtract E and carry flag from A */
void
i_SBC_A_E(z80 * pz80);
/* Subtract H and carry flag from A */
void
i_SBC_A_H(z80 * pz80);
/* Subtract and carry flag L from A */
void
i_SBC_A_L(z80 * pz80);
/* Subtract value pointed by HL and carry flag from A */
void
i_SBC_A__HL_(z80 * pz80);
/* Subtract A and carry flag from A */
void
i_SBC_A_A(z80 * pz80);
/* Logical AND B against A */
void
i_AND_B(z80 * pz80);
/* Logical AND C against A */
void
i_AND_C(z80 * pz80);
/* Logical AND D against A */
void
i_AND_D(z80 * pz80);
/* Logical AND E against A */
void
i_AND_E(z80 * pz80);
/* Logical AND H against A */
void
i_AND_H(z80 * pz80);
/* Logical AND L against A */
void
i_AND_L(z80 * pz80);
/* Logical AND value pointed by HL against A */
void
i_AND__HL_(z80 * pz80);
/* Logical AND A against A */
void
i_AND_A(z80 * pz80);
/* Logical XOR B against A */
void
i_XOR_B(z80 * pz80);
/* Logical XOR C against A */
void
i_XOR_C(z80 * pz80);
/* Logical XOR D against A */
void
i_XOR_D(z80 * pz80);
/* Logical XOR E against A */
void
i_XOR_E(z80 * pz80);
/* Logical XOR H against A */
void
i_XOR_H(z80 * pz80);
/* Logical XOR L against A */
void
i_XOR_L(z80 * pz80);
/* Logical XOR value pointed by HL against A */
void
i_XOR__HL_(z80 * pz80);
/* Logical XOR A against A */
void
i_XOR_A(z80 * pz80);
/* Logical OR B against A */
void
i_OR_B(z80 * pz80);
/* Logical OR C against A */
void
i_OR_C(z80 * pz80);
/* Logical OR D against A */
void
i_OR_D(z80 * pz80);
/* Logical OR E against A */
void
i_OR_E(z80 * pz80);
/* Logical OR H against A */
void
i_OR_H(z80 * pz80);
/* Logical OR L against A */
void
i_OR_L(z80 * pz80);
/* Logical OR value pointed by HL against A */
void
i_OR__HL_(z80 * pz80);
/* Logical OR A against A */
void
i_OR_A(z80 * pz80);
/* Compare B against A */
void
i_CP_B(z80 * pz80);
/* Compare C against A */
void
i_CP_C(z80 * pz80);
/* Compare D against A */
void
i_CP_D(z80 * pz80);
/* Compare E against A */
void
i_CP_E(z80 * pz80);
/* Compare H against A */
void
i_CP_H(z80 * pz80);
/* Compare L against A */
void
i_CP_L(z80 * pz80);
/* Compare value pointed by HL against A */
void
i_CP__HL_(z80 * pz80);
/* Compare A against A */
void
i_CP_A(z80 * pz80);
/* Return if last result was not zero */
void
i_RET_NZ(z80 * pz80);
/* Pop 16-bit value from stack into BC */
void
i_POP_BC(z80 * pz80);
/* Absolute jump to 16-bit location if last result was not zero */
void
i_JP_NZ_nn(z80 * pz80);
/* Absolute jump to 16-bit location */
void
i_JP_nn(z80 * pz80);
/* Call routine at 16-bit location if last result was not zero */
void
i_CALL_NZ_nn(z80 * pz80);
/* Push 16-bit BC onto stack */
void
i_PUSH_BC(z80 * pz80);
/* Add 8-bit immediate to A */
void
i_ADD_A_n(z80 * pz80);
/* Call routine at address 0000h */
void
i_RST_0(z80 * pz80);
/* Return if last result was zero */
void
i_RET_Z(z80 * pz80);
/* Return to calling routine */
void
i_RET(z80 * pz80);
/* Absolute jump to 16-bit location if last result was zero */
void
i_JP_Z_nn(z80 * pz80);
/* Extended operations (two-byte instruction code) */
void
i_Ext_ops(z80 * pz80);
/* Call routine at 16-bit location if last result was zero */
void
i_CALL_Z_nn(z80 * pz80);
/* Call routine at 16-bit location */
void
i_CALL_nn(z80 * pz80);
/* Add 8-bit immediate and carry to A */
void
i_ADC_A_n(z80 * pz80);
/* Call routine at address 0008h */
void
i_RST_8(z80 * pz80);
/* Return if last result caused no carry */
void
i_RET_NC(z80 * pz80);
/* Pop 16-bit value from stack into DE */
void
i_POP_DE(z80 * pz80);
/* Absolute jump to 16-bit location if last result caused no carry */
void
i_JP_NC_nn(z80 * pz80);
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80);
/* Call routine at 16-bit location if last result caused no carry */
void
i_CALL_NC_nn(z80 * pz80);
/* Push 16-bit DE onto stack */
void
i_PUSH_DE(z80 * pz80);
/* Subtract 8-bit immediate from A */
void
i_SUB_A_n(z80 * pz80);
/* Call routine at address 0010h */
void
i_RST_10(z80 * pz80);
/* Return if last result caused carry */
void
i_RET_C(z80 * pz80);
/* Enable interrupts and return to calling routine */
void
i_RETI(z80 * pz80);
/* Absolute jump to 16-bit location if last result caused carry */
void
i_JP_C_nn(z80 * pz80);
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80);
/* Call routine at 16-bit location if last result caused carry */
void
i_CALL_C_nn(z80 * pz80);
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80);
/* Subtract 8-bit immediate and carry from A */
void
i_SBC_A_n(z80 * pz80);
/* Call routine at address 0018h */
void
i_RST_18(z80 * pz80);
/* Save A at address pointed to by (FF00h + 8-bit immediate) */
void
i_LDH__n__A(z80 * pz80);
/* Pop 16-bit value from stack into HL */
void
i_POP_HL(z80 * pz80);
/* Save A at address pointed to by (FF00h + C) */
void
i_LDH__C__A(z80 * pz80);
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80);
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80);
/* Push 16-bit HL onto stack */
void
i_PUSH_HL(z80 * pz80);
/* Logical AND 8-bit immediate against A */
void
i_AND_n(z80 * pz80);
/* Call routine at address 0020h */
void
i_RST_20(z80 * pz80);
/* Add signed 8-bit immediate to SP */
void
i_ADD_SP_d(z80 * pz80);
/* Jump to 16-bit value pointed by HL */
void
i_JP__HL_(z80 * pz80);
/* Save A at given 16-bit address */
void
i_LD__nn__A(z80 * pz80);
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80);
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80);
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80);
/* Logical XOR 8-bit immediate against A */
void
i_XOR_n(z80 * pz80);
/* Call routine at address 0028h */
void
i_RST_28(z80 * pz80);
/* Load A from address pointed to by (FF00h + 8-bit immediate) */
void
i_LDH_A__n_(z80 * pz80);
/* Pop 16-bit value from stack into AF */
void
i_POP_AF(z80 * pz80);
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80);
/* DIsable interrupts */
void
i_DI(z80 * pz80);
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80);
/* Push 16-bit AF onto stack */
void
i_PUSH_AF(z80 * pz80);
/* Logical OR 8-bit immediate against A */
void
i_OR_n(z80 * pz80);
/* Call routine at address 0030h */
void
i_RST_30(z80 * pz80);
/* Add signed 8-bit immediate to SP and save result in HL */
void
i_LDHL_SP_d(z80 * pz80);
/* Copy HL to SP */
void
i_LD_SP_HL(z80 * pz80);
/* Load A from given 16-bit address */
void
i_LD_A__nn_(z80 * pz80);
/* Enable interrupts */
void
i_EI(z80 * pz80);
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80);
/* Operation removed in this CPU */
//void
//i_XX(z80 * pz80);
/* Compare 8-bit immediate against A */
void
i_CP_n(z80 * pz80);
/* Call routine at address 0038h */
void
i_RST_38(z80 * pz80);
/* Rotate B left with carry */
void
i2_RLC_B(z80 * pz80);
/* Rotate C left with carry */
void
i2_RLC_C(z80 * pz80);
/* Rotate D left with carry */
void
i2_RLC_D(z80 * pz80);
/* Rotate E left with carry */
void
i2_RLC_E(z80 * pz80);
/* Rotate H left with carry */
void
i2_RLC_H(z80 * pz80);
/* Rotate L left with carry */
void
i2_RLC_L(z80 * pz80);
/* Rotate value pointed by HL left with carry */
void
i2_RLC__HL_(z80 * pz80);
/* Rotate A left with carry */
void
i2_RLC_A(z80 * pz80);
/* Rotate B right with carry */
void
i2_RRC_B(z80 * pz80);
/* Rotate C right with carry */
void
i2_RRC_C(z80 * pz80);
/* Rotate D right with carry */
void
i2_RRC_D(z80 * pz80);
/* Rotate E right with carry */
void
i2_RRC_E(z80 * pz80);
/* Rotate H right with carry */
void
i2_RRC_H(z80 * pz80);
/* Rotate L right with carry */
void
i2_RRC_L(z80 * pz80);
/* Rotate value pointed by HL right with carry */
void
i2_RRC__HL_(z80 * pz80);
/* Rotate A right with carry */
void
i2_RRC_A(z80 * pz80);
/* Rotate B left */
void
i2_RL_B(z80 * pz80);
/* Rotate C left */
void
i2_RL_C(z80 * pz80);
/* Rotate D left */
void
i2_RL_D(z80 * pz80);
/* Rotate E left */
void
i2_RL_E(z80 * pz80);
/* Rotate H left */
void
i2_RL_H(z80 * pz80);
/* Rotate L left */
void
i2_RL_L(z80 * pz80);
/* Rotate value pointed by HL left */
void
i2_RL__HL_(z80 * pz80);
/* Rotate A left */
void
i2_RL_A(z80 * pz80);
/* Rotate B right */
void
i2_RR_B(z80 * pz80);
/* Rotate C right */
void
i2_RR_C(z80 * pz80);
/* Rotate D right */
void
i2_RR_D(z80 * pz80);
/* Rotate E right */
void
i2_RR_E(z80 * pz80);
/* Rotate H right */
void
i2_RR_H(z80 * pz80);
/* Rotate L right */
void
i2_RR_L(z80 * pz80);
/* Rotate value pointed by HL right */
void
i2_RR__HL_(z80 * pz80);
/* Rotate A right */
void
i2_RR_A(z80 * pz80);
/* Shift B left preserving sign */
void
i2_SLA_B(z80 * pz80);
/* Shift C left preserving sign */
void
i2_SLA_C(z80 * pz80);
/* Shift D left preserving sign */
void
i2_SLA_D(z80 * pz80);
/* Shift E left preserving sign */
void
i2_SLA_E(z80 * pz80);
/* Shift H left preserving sign */
void
i2_SLA_H(z80 * pz80);
/* Shift L left preserving sign */
void
i2_SLA_L(z80 * pz80);
/* Shift value pointed by HL left preserving sign */
void
i2_SLA__HL_(z80 * pz80);
/* Shift A left preserving sign */
void
i2_SLA_A(z80 * pz80);
/* Shift B right preserving sign */
void
i2_SRA_B(z80 * pz80);
/* Shift C right preserving sign */
void
i2_SRA_C(z80 * pz80);
/* Shift D right preserving sign */
void
i2_SRA_D(z80 * pz80);
/* Shift E right preserving sign */
void
i2_SRA_E(z80 * pz80);
/* Shift H right preserving sign */
void
i2_SRA_H(z80 * pz80);
/* Shift L right preserving sign */
void
i2_SRA_L(z80 * pz80);
/* Shift value pointed by HL right preserving sign */
void
i2_SRA__HL_(z80 * pz80);
/* Shift A right preserving sign */
void
i2_SRA_A(z80 * pz80);
/* Swap nybbles in B */
void
i2_SWAP_B(z80 * pz80);
/* Swap nybbles in C */
void
i2_SWAP_C(z80 * pz80);
/* Swap nybbles in D */
void
i2_SWAP_D(z80 * pz80);
/* Swap nybbles in E */
void
i2_SWAP_E(z80 * pz80);
/* Swap nybbles in H */
void
i2_SWAP_H(z80 * pz80);
/* Swap nybbles in L */
void
i2_SWAP_L(z80 * pz80);
/* Swap nybbles in value pointed by HL */
void
i2_SWAP__HL_(z80 * pz80);
/* Swap nybbles in A */
void
i2_SWAP_A(z80 * pz80);
/* Shift B right */
void
i2_SRL_B(z80 * pz80);
/* Shift C right */
void
i2_SRL_C(z80 * pz80);
/* Shift D right */
void
i2_SRL_D(z80 * pz80);
/* Shift E right */
void
i2_SRL_E(z80 * pz80);
/* Shift H right */
void
i2_SRL_H(z80 * pz80);
/* Shift L right */
void
i2_SRL_L(z80 * pz80);
/* Shift value pointed by HL right */
void
i2_SRL__HL_(z80 * pz80);
/* Shift A right */
void
i2_SRL_A(z80 * pz80);
/* Test bit 0 of B */
void
i2_BIT_0_B(z80 * pz80);
/* Test bit 0 of C */
void
i2_BIT_0_C(z80 * pz80);
/* Test bit 0 of D */
void
i2_BIT_0_D(z80 * pz80);
/* Test bit 0 of E */
void
i2_BIT_0_E(z80 * pz80);
/* Test bit 0 of H */
void
i2_BIT_0_H(z80 * pz80);
/* Test bit 0 of L */
void
i2_BIT_0_L(z80 * pz80);
/* Test bit 0 of value pointed by HL */
void
i2_BIT_0__HL_(z80 * pz80);
/* Test bit 0 of A */
void
i2_BIT_0_A(z80 * pz80);
/* Test bit 1 of B */
void
i2_BIT_1_B(z80 * pz80);
/* Test bit 1 of C */
void
i2_BIT_1_C(z80 * pz80);
/* Test bit 1 of D */
void
i2_BIT_1_D(z80 * pz80);
/* Test bit 1 of E */
void
i2_BIT_1_E(z80 * pz80);
/* Test bit 1 of H */
void
i2_BIT_1_H(z80 * pz80);
/* Test bit 1 of L */
void
i2_BIT_1_L(z80 * pz80);
/* Test bit 1 of value pointed by HL */
void
i2_BIT_1__HL_(z80 * pz80);
/* Test bit 1 of A */
void
i2_BIT_1_A(z80 * pz80);
/* Test bit 2 of B */
void
i2_BIT_2_B(z80 * pz80);
/* Test bit 2 of C */
void
i2_BIT_2_C(z80 * pz80);
/* Test bit 2 of D */
void
i2_BIT_2_D(z80 * pz80);
/* Test bit 2 of E */
void
i2_BIT_2_E(z80 * pz80);
/* Test bit 2 of H */
void
i2_BIT_2_H(z80 * pz80);
/* Test bit 2 of L */
void
i2_BIT_2_L(z80 * pz80);
/* Test bit 2 of value pointed by HL */
void
i2_BIT_2__HL_(z80 * pz80);
/* Test bit 2 of A */
void
i2_BIT_2_A(z80 * pz80);
/* Test bit 3 of B */
void
i2_BIT_3_B(z80 * pz80);
/* Test bit 3 of C */
void
i2_BIT_3_C(z80 * pz80);
/* Test bit 3 of D */
void
i2_BIT_3_D(z80 * pz80);
/* Test bit 3 of E */
void
i2_BIT_3_E(z80 * pz80);
/* Test bit 3 of H */
void
i2_BIT_3_H(z80 * pz80);
/* Test bit 3 of L */
void
i2_BIT_3_L(z80 * pz80);
/* Test bit 3 of value pointed by HL */
void
i2_BIT_3__HL_(z80 * pz80);
/* Test bit 3 of A */
void
i2_BIT_3_A(z80 * pz80);
/* Test bit 4 of B */
void
i2_BIT_4_B(z80 * pz80);
/* Test bit 4 of C */
void
i2_BIT_4_C(z80 * pz80);
/* Test bit 4 of D */
void
i2_BIT_4_D(z80 * pz80);
/* Test bit 4 of E */
void
i2_BIT_4_E(z80 * pz80);
/* Test bit 4 of H */
void
i2_BIT_4_H(z80 * pz80);
/* Test bit 4 of L */
void
i2_BIT_4_L(z80 * pz80);
/* Test bit 4 of value pointed by HL */
void
i2_BIT_4__HL_(z80 * pz80);
/* Test bit 4 of A */
void
i2_BIT_4_A(z80 * pz80);
/* Test bit 5 of B */
void
i2_BIT_5_B(z80 * pz80);
/* Test bit 5 of C */
void
i2_BIT_5_C(z80 * pz80);
/* Test bit 5 of D */
void
i2_BIT_5_D(z80 * pz80);
/* Test bit 5 of E */
void
i2_BIT_5_E(z80 * pz80);
/* Test bit 5 of H */
void
i2_BIT_5_H(z80 * pz80);
/* Test bit 5 of L */
void
i2_BIT_5_L(z80 * pz80);
/* Test bit 5 of value pointed by HL */
void
i2_BIT_5__HL_(z80 * pz80);
/* Test bit 5 of A */
void
i2_BIT_5_A(z80 * pz80);
/* Test bit 6 of B */
void
i2_BIT_6_B(z80 * pz80);
/* Test bit 6 of C */
void
i2_BIT_6_C(z80 * pz80);
/* Test bit 6 of D */
void
i2_BIT_6_D(z80 * pz80);
/* Test bit 6 of E */
void
i2_BIT_6_E(z80 * pz80);
/* Test bit 6 of H */
void
i2_BIT_6_H(z80 * pz80);
/* Test bit 6 of L */
void
i2_BIT_6_L(z80 * pz80);
/* Test bit 6 of value pointed by HL */
void
i2_BIT_6__HL_(z80 * pz80);
/* Test bit 6 of A */
void
i2_BIT_6_A(z80 * pz80);
/* Test bit 7 of B */
void
i2_BIT_7_B(z80 * pz80);
/* Test bit 7 of C */
void
i2_BIT_7_C(z80 * pz80);
/* Test bit 7 of D */
void
i2_BIT_7_D(z80 * pz80);
/* Test bit 7 of E */
void
i2_BIT_7_E(z80 * pz80);
/* Test bit 7 of H */
void
i2_BIT_7_H(z80 * pz80);
/* Test bit 7 of L */
void
i2_BIT_7_L(z80 * pz80);
/* Test bit 7 of value pointed by HL */
void
i2_BIT_7__HL_(z80 * pz80);
/* Test bit 7 of A */
void
i2_BIT_7_A(z80 * pz80);
/* Clear (reset) bit 0 of B */
void
i2_RES_0_B(z80 * pz80);
/* Clear (reset) bit 0 of C */
void
i2_RES_0_C(z80 * pz80);
/* Clear (reset) bit 0 of D */
void
i2_RES_0_D(z80 * pz80);
/* Clear (reset) bit 0 of E */
void
i2_RES_0_E(z80 * pz80);
/* Clear (reset) bit 0 of H */
void
i2_RES_0_H(z80 * pz80);
/* Clear (reset) bit 0 of L */
void
i2_RES_0_L(z80 * pz80);
/* Clear (reset) bit 0 of value pointed by HL */
void
i2_RES_0__HL_(z80 * pz80);
/* Clear (reset) bit 0 of A */
void
i2_RES_0_A(z80 * pz80);
/* Clear (reset) bit 1 of B */
void
i2_RES_1_B(z80 * pz80);
/* Clear (reset) bit 1 of C */
void
i2_RES_1_C(z80 * pz80);
/* Clear (reset) bit 1 of D */
void
i2_RES_1_D(z80 * pz80);
/* Clear (reset) bit 1 of E */
void
i2_RES_1_E(z80 * pz80);
/* Clear (reset) bit 1 of H */
void
i2_RES_1_H(z80 * pz80);
/* Clear (reset) bit 1 of L */
void
i2_RES_1_L(z80 * pz80);
/* Clear (reset) bit 1 of value pointed by HL */
void
i2_RES_1__HL_(z80 * pz80);
/* Clear (reset) bit 1 of A */
void
i2_RES_1_A(z80 * pz80);
/* Clear (reset) bit 2 of B */
void
i2_RES_2_B(z80 * pz80);
/* Clear (reset) bit 2 of C */
void
i2_RES_2_C(z80 * pz80);
/* Clear (reset) bit 2 of D */
void
i2_RES_2_D(z80 * pz80);
/* Clear (reset) bit 2 of E */
void
i2_RES_2_E(z80 * pz80);
/* Clear (reset) bit 2 of H */
void
i2_RES_2_H(z80 * pz80);
/* Clear (reset) bit 2 of L */
void
i2_RES_2_L(z80 * pz80);
/* Clear (reset) bit 2 of value pointed by HL */
void
i2_RES_2__HL_(z80 * pz80);
/* Clear (reset) bit 2 of A */
void
i2_RES_2_A(z80 * pz80);
/* Clear (reset) bit 3 of B */
void
i2_RES_3_B(z80 * pz80);
/* Clear (reset) bit 3 of C */
void
i2_RES_3_C(z80 * pz80);
/* Clear (reset) bit 3 of D */
void
i2_RES_3_D(z80 * pz80);
/* Clear (reset) bit 3 of E */
void
i2_RES_3_E(z80 * pz80);
/* Clear (reset) bit 3 of H */
void
i2_RES_3_H(z80 * pz80);
/* Clear (reset) bit 3 of L */
void
i2_RES_3_L(z80 * pz80);
/* Clear (reset) bit 3 of value pointed by HL */
void
i2_RES_3__HL_(z80 * pz80);
/* Clear (reset) bit 3 of A */
void
i2_RES_3_A(z80 * pz80);
/* Clear (reset) bit 4 of B */
void
i2_RES_4_B(z80 * pz80);
/* Clear (reset) bit 4 of C */
void
i2_RES_4_C(z80 * pz80);
/* Clear (reset) bit 4 of D */
void
i2_RES_4_D(z80 * pz80);
/* Clear (reset) bit 4 of E */
void
i2_RES_4_E(z80 * pz80);
/* Clear (reset) bit 4 of H */
void
i2_RES_4_H(z80 * pz80);
/* Clear (reset) bit 4 of L */
void
i2_RES_4_L(z80 * pz80);
/* Clear (reset) bit 4 of value pointed by HL */
void
i2_RES_4__HL_(z80 * pz80);
/* Clear (reset) bit 4 of A */
void
i2_RES_4_A(z80 * pz80);
/* Clear (reset) bit 5 of B */
void
i2_RES_5_B(z80 * pz80);
/* Clear (reset) bit 5 of C */
void
i2_RES_5_C(z80 * pz80);
/* Clear (reset) bit 5 of D */
void
i2_RES_5_D(z80 * pz80);
/* Clear (reset) bit 5 of E */
void
i2_RES_5_E(z80 * pz80);
/* Clear (reset) bit 5 of H */
void
i2_RES_5_H(z80 * pz80);
/* Clear (reset) bit 5 of L */
void
i2_RES_5_L(z80 * pz80);
/* Clear (reset) bit 5 of value pointed by HL */
void
i2_RES_5__HL_(z80 * pz80);
/* Clear (reset) bit 5 of A */
void
i2_RES_5_A(z80 * pz80);
/* Clear (reset) bit 6 of B */
void
i2_RES_6_B(z80 * pz80);
/* Clear (reset) bit 6 of C */
void
i2_RES_6_C(z80 * pz80);
/* Clear (reset) bit 6 of D */
void
i2_RES_6_D(z80 * pz80);
/* Clear (reset) bit 6 of E */
void
i2_RES_6_E(z80 * pz80);
/* Clear (reset) bit 6 of H */
void
i2_RES_6_H(z80 * pz80);
/* Clear (reset) bit 6 of L */
void
i2_RES_6_L(z80 * pz80);
/* Clear (reset) bit 6 of value pointed by HL */
void
i2_RES_6__HL_(z80 * pz80);
/* Clear (reset) bit 6 of A */
void
i2_RES_6_A(z80 * pz80);
/* Clear (reset) bit 7 of B */
void
i2_RES_7_B(z80 * pz80);
/* Clear (reset) bit 7 of C */
void
i2_RES_7_C(z80 * pz80);
/* Clear (reset) bit 7 of D */
void
i2_RES_7_D(z80 * pz80);
/* Clear (reset) bit 7 of E */
void
i2_RES_7_E(z80 * pz80);
/* Clear (reset) bit 7 of H */
void
i2_RES_7_H(z80 * pz80);
/* Clear (reset) bit 7 of L */
void
i2_RES_7_L(z80 * pz80);
/* Clear (reset) bit 7 of value pointed by HL */
void
i2_RES_7__HL_(z80 * pz80);
/* Clear (reset) bit 7 of A */
void
i2_RES_7_A(z80 * pz80);
/* Set bit 0 of B */
void
i2_SET_0_B(z80 * pz80);
/* Set bit 0 of C */
void
i2_SET_0_C(z80 * pz80);
/* Set bit 0 of D */
void
i2_SET_0_D(z80 * pz80);
/* Set bit 0 of E */
void
i2_SET_0_E(z80 * pz80);
/* Set bit 0 of H */
void
i2_SET_0_H(z80 * pz80);
/* Set bit 0 of L */
void
i2_SET_0_L(z80 * pz80);
/* Set bit 0 of value pointed by HL */
void
i2_SET_0__HL_(z80 * pz80);
/* Set bit 0 of A */
void
i2_SET_0_A(z80 * pz80);
/* Set bit 1 of B */
void
i2_SET_1_B(z80 * pz80);
/* Set bit 1 of C */
void
i2_SET_1_C(z80 * pz80);
/* Set bit 1 of D */
void
i2_SET_1_D(z80 * pz80);
/* Set bit 1 of E */
void
i2_SET_1_E(z80 * pz80);
/* Set bit 1 of H */
void
i2_SET_1_H(z80 * pz80);
/* Set bit 1 of L */
void
i2_SET_1_L(z80 * pz80);
/* Set bit 1 of value pointed by HL */
void
i2_SET_1__HL_(z80 * pz80);
/* Set bit 1 of A */
void
i2_SET_1_A(z80 * pz80);
/* Set bit 2 of B */
void
i2_SET_2_B(z80 * pz80);
/* Set bit 2 of C */
void
i2_SET_2_C(z80 * pz80);
/* Set bit 2 of D */
void
i2_SET_2_D(z80 * pz80);
/* Set bit 2 of E */
void
i2_SET_2_E(z80 * pz80);
/* Set bit 2 of H */
void
i2_SET_2_H(z80 * pz80);
/* Set bit 2 of L */
void
i2_SET_2_L(z80 * pz80);
/* Set bit 2 of value pointed by HL */
void
i2_SET_2__HL_(z80 * pz80);
/* Set bit 2 of A */
void
i2_SET_2_A(z80 * pz80);
/* Set bit 3 of B */
void
i2_SET_3_B(z80 * pz80);
/* Set bit 3 of C */
void
i2_SET_3_C(z80 * pz80);
/* Set bit 3 of D */
void
i2_SET_3_D(z80 * pz80);
/* Set bit 3 of E */
void
i2_SET_3_E(z80 * pz80);
/* Set bit 3 of H */
void
i2_SET_3_H(z80 * pz80);
/* Set bit 3 of L */
void
i2_SET_3_L(z80 * pz80);
/* Set bit 3 of value pointed by HL */
void
i2_SET_3__HL_(z80 * pz80);
/* Set bit 3 of A */
void
i2_SET_3_A(z80 * pz80);
/* Set bit 4 of B */
void
i2_SET_4_B(z80 * pz80);
/* Set bit 4 of C */
void
i2_SET_4_C(z80 * pz80);
/* Set bit 4 of D */
void
i2_SET_4_D(z80 * pz80);
/* Set bit 4 of E */
void
i2_SET_4_E(z80 * pz80);
/* Set bit 4 of H */
void
i2_SET_4_H(z80 * pz80);
/* Set bit 4 of L */
void
i2_SET_4_L(z80 * pz80);
/* Set bit 4 of value pointed by HL */
void
i2_SET_4__HL_(z80 * pz80);
/* Set bit 4 of A */
void
i2_SET_4_A(z80 * pz80);
/* Set bit 5 of B */
void
i2_SET_5_B(z80 * pz80);
/* Set bit 5 of C */
void
i2_SET_5_C(z80 * pz80);
/* Set bit 5 of D */
void
i2_SET_5_D(z80 * pz80);
/* Set bit 5 of E */
void
i2_SET_5_E(z80 * pz80);
/* Set bit 5 of H */
void
i2_SET_5_H(z80 * pz80);
/* Set bit 5 of L */
void
i2_SET_5_L(z80 * pz80);
/* Set bit 5 of value pointed by HL */
void
i2_SET_5__HL_(z80 * pz80);
/* Set bit 5 of A */
void
i2_SET_5_A(z80 * pz80);
/* Set bit 6 of B */
void
i2_SET_6_B(z80 * pz80);
/* Set bit 6 of C */
void
i2_SET_6_C(z80 * pz80);
/* Set bit 6 of D */
void
i2_SET_6_D(z80 * pz80);
/* Set bit 6 of E */
void
i2_SET_6_E(z80 * pz80);
/* Set bit 6 of H */
void
i2_SET_6_H(z80 * pz80);
/* Set bit 6 of L */
void
i2_SET_6_L(z80 * pz80);
/* Set bit 6 of value pointed by HL */
void
i2_SET_6__HL_(z80 * pz80);
/* Set bit 6 of A */
void
i2_SET_6_A(z80 * pz80);
/* Set bit 7 of B */
void
i2_SET_7_B(z80 * pz80);
/* Set bit 7 of C */
void
i2_SET_7_C(z80 * pz80);
/* Set bit 7 of D */
void
i2_SET_7_D(z80 * pz80);
/* Set bit 7 of E */
void
i2_SET_7_E(z80 * pz80);
/* Set bit 7 of H */
void
i2_SET_7_H(z80 * pz80);
/* Set bit 7 of L */
void
i2_SET_7_L(z80 * pz80);
/* Set bit 7 of value pointed by HL */
void
i2_SET_7__HL_(z80 * pz80);
/* Set bit 7 of A */
void
i2_SET_7_A(z80 * pz80);


#endif
