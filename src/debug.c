#include "debug.h"

void
fprintCPU(FILE*out,z80*pz80){

	fprintf(out,
	"Registers\n"\
	"\tA: %02x B: %02x C: %02x D: %02x\n"\
	"E: %02x H: %02x L: %02x\n PC: %04x\n"\
	"Status Flags \n"\
	"z: %d s: %d c: %d hc: %d \n"\
	"ime %d \n",
	(int)pz80->registers[REGA],(int)pz80->registers[REGB],(int)pz80->registers[REGC],
	(int)pz80->registers[REGD],(int)pz80->registers[REGE],(int)pz80->registers[REGH],
	(int)pz80->registers[REGL],(int)pz80->registers16[PC],(int)getFlag(pz80->registers[REGF],ZERO),
	(int)getFlag(pz80->registers[REGF],SUB),(int)getFlag(pz80->registers[REGF],CARRY),
	(int)getFlag(pz80->registers[REGF],HALFCARRY),
	pz80->ime
	);
}




/* this function was autogenerated by Utility/generateDissasmble.py */
	const char * dissasemble(uint8_t opcode,int secondary){
	if(secondary){
	
	switch(opcode){
		case 0x00:
			return "i2_RLC_B";
		case 0x01:
			return "i2_RLC_C";
		case 0x02:
			return "i2_RLC_D";
		case 0x03:
			return "i2_RLC_E";
		case 0x04:
			return "i2_RLC_H";
		case 0x05:
			return "i2_RLC_L";
		case 0x06:
			return "i2_RLC__HL_";
		case 0x07:
			return "i2_RLC_A";
		case 0x08:
			return "i2_RRC_B";
		case 0x09:
			return "i2_RRC_C";
		case 0x0A:
			return "i2_RRC_D";
		case 0x0B:
			return "i2_RRC_E";
		case 0x0C:
			return "i2_RRC_H";
		case 0x0D:
			return "i2_RRC_L";
		case 0x0E:
			return "i2_RRC__HL_";
		case 0x0F:
			return "i2_RRC_A";
		case 0x10:
			return "i2_RL_B";
		case 0x11:
			return "i2_RL_C";
		case 0x12:
			return "i2_RL_D";
		case 0x13:
			return "i2_RL_E";
		case 0x14:
			return "i2_RL_H";
		case 0x15:
			return "i2_RL_L";
		case 0x16:
			return "i2_RL__HL_";
		case 0x17:
			return "i2_RL_A";
		case 0x18:
			return "i2_RR_B";
		case 0x19:
			return "i2_RR_C";
		case 0x1A:
			return "i2_RR_D";
		case 0x1B:
			return "i2_RR_E";
		case 0x1C:
			return "i2_RR_H";
		case 0x1D:
			return "i2_RR_L";
		case 0x1E:
			return "i2_RR__HL_";
		case 0x1F:
			return "i2_RR_A";
		case 0x20:
			return "i2_SLA_B";
		case 0x21:
			return "i2_SLA_C";
		case 0x22:
			return "i2_SLA_D";
		case 0x23:
			return "i2_SLA_E";
		case 0x24:
			return "i2_SLA_H";
		case 0x25:
			return "i2_SLA_L";
		case 0x26:
			return "i2_SLA__HL_";
		case 0x27:
			return "i2_SLA_A";
		case 0x28:
			return "i2_SRA_B";
		case 0x29:
			return "i2_SRA_C";
		case 0x2A:
			return "i2_SRA_D";
		case 0x2B:
			return "i2_SRA_E";
		case 0x2C:
			return "i2_SRA_H";
		case 0x2D:
			return "i2_SRA_L";
		case 0x2E:
			return "i2_SRA__HL_";
		case 0x2F:
			return "i2_SRA_A";
		case 0x30:
			return "i2_SWAP_B";
		case 0x31:
			return "i2_SWAP_C";
		case 0x32:
			return "i2_SWAP_D";
		case 0x33:
			return "i2_SWAP_E";
		case 0x34:
			return "i2_SWAP_H";
		case 0x35:
			return "i2_SWAP_L";
		case 0x36:
			return "i2_SWAP__HL_";
		case 0x37:
			return "i2_SWAP_A";
		case 0x38:
			return "i2_SRL_B";
		case 0x39:
			return "i2_SRL_C";
		case 0x3A:
			return "i2_SRL_D";
		case 0x3B:
			return "i2_SRL_E";
		case 0x3C:
			return "i2_SRL_H";
		case 0x3D:
			return "i2_SRL_L";
		case 0x3E:
			return "i2_SRL__HL_";
		case 0x3F:
			return "i2_SRL_A";
		case 0x40:
			return "i2_BIT_0_B";
		case 0x41:
			return "i2_BIT_0_C";
		case 0x42:
			return "i2_BIT_0_D";
		case 0x43:
			return "i2_BIT_0_E";
		case 0x44:
			return "i2_BIT_0_H";
		case 0x45:
			return "i2_BIT_0_L";
		case 0x46:
			return "i2_BIT_0__HL_";
		case 0x47:
			return "i2_BIT_0_A";
		case 0x48:
			return "i2_BIT_1_B";
		case 0x49:
			return "i2_BIT_1_C";
		case 0x4A:
			return "i2_BIT_1_D";
		case 0x4B:
			return "i2_BIT_1_E";
		case 0x4C:
			return "i2_BIT_1_H";
		case 0x4D:
			return "i2_BIT_1_L";
		case 0x4E:
			return "i2_BIT_1__HL_";
		case 0x4F:
			return "i2_BIT_1_A";
		case 0x50:
			return "i2_BIT_2_B";
		case 0x51:
			return "i2_BIT_2_C";
		case 0x52:
			return "i2_BIT_2_D";
		case 0x53:
			return "i2_BIT_2_E";
		case 0x54:
			return "i2_BIT_2_H";
		case 0x55:
			return "i2_BIT_2_L";
		case 0x56:
			return "i2_BIT_2__HL_";
		case 0x57:
			return "i2_BIT_2_A";
		case 0x58:
			return "i2_BIT_3_B";
		case 0x59:
			return "i2_BIT_3_C";
		case 0x5A:
			return "i2_BIT_3_D";
		case 0x5B:
			return "i2_BIT_3_E";
		case 0x5C:
			return "i2_BIT_3_H";
		case 0x5D:
			return "i2_BIT_3_L";
		case 0x5E:
			return "i2_BIT_3__HL_";
		case 0x5F:
			return "i2_BIT_3_A";
		case 0x60:
			return "i2_BIT_4_B";
		case 0x61:
			return "i2_BIT_4_C";
		case 0x62:
			return "i2_BIT_4_D";
		case 0x63:
			return "i2_BIT_4_E";
		case 0x64:
			return "i2_BIT_4_H";
		case 0x65:
			return "i2_BIT_4_L";
		case 0x66:
			return "i2_BIT_4__HL_";
		case 0x67:
			return "i2_BIT_4_A";
		case 0x68:
			return "i2_BIT_5_B";
		case 0x69:
			return "i2_BIT_5_C";
		case 0x6A:
			return "i2_BIT_5_D";
		case 0x6B:
			return "i2_BIT_5_E";
		case 0x6C:
			return "i2_BIT_5_H";
		case 0x6D:
			return "i2_BIT_5_L";
		case 0x6E:
			return "i2_BIT_5__HL_";
		case 0x6F:
			return "i2_BIT_5_A";
		case 0x70:
			return "i2_BIT_6_B";
		case 0x71:
			return "i2_BIT_6_C";
		case 0x72:
			return "i2_BIT_6_D";
		case 0x73:
			return "i2_BIT_6_E";
		case 0x74:
			return "i2_BIT_6_H";
		case 0x75:
			return "i2_BIT_6_L";
		case 0x76:
			return "i2_BIT_6__HL_";
		case 0x77:
			return "i2_BIT_6_A";
		case 0x78:
			return "i2_BIT_7_B";
		case 0x79:
			return "i2_BIT_7_C";
		case 0x7A:
			return "i2_BIT_7_D";
		case 0x7B:
			return "i2_BIT_7_E";
		case 0x7C:
			return "i2_BIT_7_H";
		case 0x7D:
			return "i2_BIT_7_L";
		case 0x7E:
			return "i2_BIT_7__HL_";
		case 0x7F:
			return "i2_BIT_7_A";
		case 0x80:
			return "i2_RES_0_B";
		case 0x81:
			return "i2_RES_0_C";
		case 0x82:
			return "i2_RES_0_D";
		case 0x83:
			return "i2_RES_0_E";
		case 0x84:
			return "i2_RES_0_H";
		case 0x85:
			return "i2_RES_0_L";
		case 0x86:
			return "i2_RES_0__HL_";
		case 0x87:
			return "i2_RES_0_A";
		case 0x88:
			return "i2_RES_1_B";
		case 0x89:
			return "i2_RES_1_C";
		case 0x8A:
			return "i2_RES_1_D";
		case 0x8B:
			return "i2_RES_1_E";
		case 0x8C:
			return "i2_RES_1_H";
		case 0x8D:
			return "i2_RES_1_L";
		case 0x8E:
			return "i2_RES_1__HL_";
		case 0x8F:
			return "i2_RES_1_A";
		case 0x90:
			return "i2_RES_2_B";
		case 0x91:
			return "i2_RES_2_C";
		case 0x92:
			return "i2_RES_2_D";
		case 0x93:
			return "i2_RES_2_E";
		case 0x94:
			return "i2_RES_2_H";
		case 0x95:
			return "i2_RES_2_L";
		case 0x96:
			return "i2_RES_2__HL_";
		case 0x97:
			return "i2_RES_2_A";
		case 0x98:
			return "i2_RES_3_B";
		case 0x99:
			return "i2_RES_3_C";
		case 0x9A:
			return "i2_RES_3_D";
		case 0x9B:
			return "i2_RES_3_E";
		case 0x9C:
			return "i2_RES_3_H";
		case 0x9D:
			return "i2_RES_3_L";
		case 0x9E:
			return "i2_RES_3__HL_";
		case 0x9F:
			return "i2_RES_3_A";
		case 0xA0:
			return "i2_RES_4_B";
		case 0xA1:
			return "i2_RES_4_C";
		case 0xA2:
			return "i2_RES_4_D";
		case 0xA3:
			return "i2_RES_4_E";
		case 0xA4:
			return "i2_RES_4_H";
		case 0xA5:
			return "i2_RES_4_L";
		case 0xA6:
			return "i2_RES_4__HL_";
		case 0xA7:
			return "i2_RES_4_A";
		case 0xA8:
			return "i2_RES_5_B";
		case 0xA9:
			return "i2_RES_5_C";
		case 0xAA:
			return "i2_RES_5_D";
		case 0xAB:
			return "i2_RES_5_E";
		case 0xAC:
			return "i2_RES_5_H";
		case 0xAD:
			return "i2_RES_5_L";
		case 0xAE:
			return "i2_RES_5__HL_";
		case 0xAF:
			return "i2_RES_5_A";
		case 0xB0:
			return "i2_RES_6_B";
		case 0xB1:
			return "i2_RES_6_C";
		case 0xB2:
			return "i2_RES_6_D";
		case 0xB3:
			return "i2_RES_6_E";
		case 0xB4:
			return "i2_RES_6_H";
		case 0xB5:
			return "i2_RES_6_L";
		case 0xB6:
			return "i2_RES_6__HL_";
		case 0xB7:
			return "i2_RES_6_A";
		case 0xB8:
			return "i2_RES_7_B";
		case 0xB9:
			return "i2_RES_7_C";
		case 0xBA:
			return "i2_RES_7_D";
		case 0xBB:
			return "i2_RES_7_E";
		case 0xBC:
			return "i2_RES_7_H";
		case 0xBD:
			return "i2_RES_7_L";
		case 0xBE:
			return "i2_RES_7__HL_";
		case 0xBF:
			return "i2_RES_7_A";
		case 0xC0:
			return "i2_SET_0_B";
		case 0xC1:
			return "i2_SET_0_C";
		case 0xC2:
			return "i2_SET_0_D";
		case 0xC3:
			return "i2_SET_0_E";
		case 0xC4:
			return "i2_SET_0_H";
		case 0xC5:
			return "i2_SET_0_L";
		case 0xC6:
			return "i2_SET_0__HL_";
		case 0xC7:
			return "i2_SET_0_A";
		case 0xC8:
			return "i2_SET_1_B";
		case 0xC9:
			return "i2_SET_1_C";
		case 0xCA:
			return "i2_SET_1_D";
		case 0xCB:
			return "i2_SET_1_E";
		case 0xCC:
			return "i2_SET_1_H";
		case 0xCD:
			return "i2_SET_1_L";
		case 0xCE:
			return "i2_SET_1__HL_";
		case 0xCF:
			return "i2_SET_1_A";
		case 0xD0:
			return "i2_SET_2_B";
		case 0xD1:
			return "i2_SET_2_C";
		case 0xD2:
			return "i2_SET_2_D";
		case 0xD3:
			return "i2_SET_2_E";
		case 0xD4:
			return "i2_SET_2_H";
		case 0xD5:
			return "i2_SET_2_L";
		case 0xD6:
			return "i2_SET_2__HL_";
		case 0xD7:
			return "i2_SET_2_A";
		case 0xD8:
			return "i2_SET_3_B";
		case 0xD9:
			return "i2_SET_3_C";
		case 0xDA:
			return "i2_SET_3_D";
		case 0xDB:
			return "i2_SET_3_E";
		case 0xDC:
			return "i2_SET_3_H";
		case 0xDD:
			return "i2_SET_3_L";
		case 0xDE:
			return "i2_SET_3__HL_";
		case 0xDF:
			return "i2_SET_3_A";
		case 0xE0:
			return "i2_SET_4_B";
		case 0xE1:
			return "i2_SET_4_C";
		case 0xE2:
			return "i2_SET_4_D";
		case 0xE3:
			return "i2_SET_4_E";
		case 0xE4:
			return "i2_SET_4_H";
		case 0xE5:
			return "i2_SET_4_L";
		case 0xE6:
			return "i2_SET_4__HL_";
		case 0xE7:
			return "i2_SET_4_A";
		case 0xE8:
			return "i2_SET_5_B";
		case 0xE9:
			return "i2_SET_5_C";
		case 0xEA:
			return "i2_SET_5_D";
		case 0xEB:
			return "i2_SET_5_E";
		case 0xEC:
			return "i2_SET_5_H";
		case 0xED:
			return "i2_SET_5_L";
		case 0xEE:
			return "i2_SET_5__HL_";
		case 0xEF:
			return "i2_SET_5_A";
		case 0xF0:
			return "i2_SET_6_B";
		case 0xF1:
			return "i2_SET_6_C";
		case 0xF2:
			return "i2_SET_6_D";
		case 0xF3:
			return "i2_SET_6_E";
		case 0xF4:
			return "i2_SET_6_H";
		case 0xF5:
			return "i2_SET_6_L";
		case 0xF6:
			return "i2_SET_6__HL_";
		case 0xF7:
			return "i2_SET_6_A";
		case 0xF8:
			return "i2_SET_7_B";
		case 0xF9:
			return "i2_SET_7_C";
		case 0xFA:
			return "i2_SET_7_D";
		case 0xFB:
			return "i2_SET_7_E";
		case 0xFC:
			return "i2_SET_7_H";
		case 0xFD:
			return "i2_SET_7_L";
		case 0xFE:
			return "i2_SET_7__HL_";
		case 0xFF:
			return "i2_SET_7_A";

	}

	}else{
	
	switch(opcode){
		case 0x00:
			return "i_NOP";
		case 0x01:
			return "i_LD_BC_nn";
		case 0x02:
			return "i_LD__BC__A";
		case 0x03:
			return "i_INC_BC";
		case 0x04:
			return "i_INC_B";
		case 0x05:
			return "i_DEC_B";
		case 0x06:
			return "i_LD_B_n";
		case 0x07:
			return "i_RLC_A";
		case 0x08:
			return "i_LD__nn__SP";
		case 0x09:
			return "i_ADD_HL_BC";
		case 0x0A:
			return "i_LD_A__BC_";
		case 0x0B:
			return "i_DEC_BC";
		case 0x0C:
			return "i_INC_C";
		case 0x0D:
			return "i_DEC_C";
		case 0x0E:
			return "i_LD_C_n";
		case 0x0F:
			return "i_RRC_A";
		case 0x10:
			return "i_STOP";
		case 0x11:
			return "i_LD_DE_nn";
		case 0x12:
			return "i_LD__DE__A";
		case 0x13:
			return "i_INC_DE";
		case 0x14:
			return "i_INC_D";
		case 0x15:
			return "i_DEC_D";
		case 0x16:
			return "i_LD_D_n";
		case 0x17:
			return "i_RL_A";
		case 0x18:
			return "i_JR_n";
		case 0x19:
			return "i_ADD_HL_DE";
		case 0x1A:
			return "i_LD_A__DE_";
		case 0x1B:
			return "i_DEC_DE";
		case 0x1C:
			return "i_INC_E";
		case 0x1D:
			return "i_DEC_E";
		case 0x1E:
			return "i_LD_E_n";
		case 0x1F:
			return "i_RR_A";
		case 0x20:
			return "i_JR_NZ_n";
		case 0x21:
			return "i_LD_HL_nn";
		case 0x22:
			return "i_LDI__HL__A";
		case 0x23:
			return "i_INC_HL";
		case 0x24:
			return "i_INC_H";
		case 0x25:
			return "i_DEC_H";
		case 0x26:
			return "i_LD_H_n";
		case 0x27:
			return "i_DAA";
		case 0x28:
			return "i_JR_Z_n";
		case 0x29:
			return "i_ADD_HL_HL";
		case 0x2A:
			return "i_LDI_A__HL_";
		case 0x2B:
			return "i_DEC_HL";
		case 0x2C:
			return "i_INC_L";
		case 0x2D:
			return "i_DEC_L";
		case 0x2E:
			return "i_LD_L_n";
		case 0x2F:
			return "i_CPL";
		case 0x30:
			return "i_JR_NC_n";
		case 0x31:
			return "i_LD_SP_nn";
		case 0x32:
			return "i_LDD__HL__A";
		case 0x33:
			return "i_INC_SP";
		case 0x34:
			return "i_INC__HL_";
		case 0x35:
			return "i_DEC__HL_";
		case 0x36:
			return "i_LD__HL__n";
		case 0x37:
			return "i_SCF";
		case 0x38:
			return "i_JR_C_n";
		case 0x39:
			return "i_ADD_HL_SP";
		case 0x3A:
			return "i_LDD_A__HL_";
		case 0x3B:
			return "i_DEC_SP";
		case 0x3C:
			return "i_INC_A";
		case 0x3D:
			return "i_DEC_A";
		case 0x3E:
			return "i_LD_A_n";
		case 0x3F:
			return "i_CCF";
		case 0x40:
			return "i_LD_B_B";
		case 0x41:
			return "i_LD_B_C";
		case 0x42:
			return "i_LD_B_D";
		case 0x43:
			return "i_LD_B_E";
		case 0x44:
			return "i_LD_B_H";
		case 0x45:
			return "i_LD_B_L";
		case 0x46:
			return "i_LD_B__HL_";
		case 0x47:
			return "i_LD_B_A";
		case 0x48:
			return "i_LD_C_B";
		case 0x49:
			return "i_LD_C_C";
		case 0x4A:
			return "i_LD_C_D";
		case 0x4B:
			return "i_LD_C_E";
		case 0x4C:
			return "i_LD_C_H";
		case 0x4D:
			return "i_LD_C_L";
		case 0x4E:
			return "i_LD_C__HL_";
		case 0x4F:
			return "i_LD_C_A";
		case 0x50:
			return "i_LD_D_B";
		case 0x51:
			return "i_LD_D_C";
		case 0x52:
			return "i_LD_D_D";
		case 0x53:
			return "i_LD_D_E";
		case 0x54:
			return "i_LD_D_H";
		case 0x55:
			return "i_LD_D_L";
		case 0x56:
			return "i_LD_D__HL_";
		case 0x57:
			return "i_LD_D_A";
		case 0x58:
			return "i_LD_E_B";
		case 0x59:
			return "i_LD_E_C";
		case 0x5A:
			return "i_LD_E_D";
		case 0x5B:
			return "i_LD_E_E";
		case 0x5C:
			return "i_LD_E_H";
		case 0x5D:
			return "i_LD_E_L";
		case 0x5E:
			return "i_LD_E__HL_";
		case 0x5F:
			return "i_LD_E_A";
		case 0x60:
			return "i_LD_H_B";
		case 0x61:
			return "i_LD_H_C";
		case 0x62:
			return "i_LD_H_D";
		case 0x63:
			return "i_LD_H_E";
		case 0x64:
			return "i_LD_H_H";
		case 0x65:
			return "i_LD_H_L";
		case 0x66:
			return "i_LD_H__HL_";
		case 0x67:
			return "i_LD_H_A";
		case 0x68:
			return "i_LD_L_B";
		case 0x69:
			return "i_LD_L_C";
		case 0x6A:
			return "i_LD_L_D";
		case 0x6B:
			return "i_LD_L_E";
		case 0x6C:
			return "i_LD_L_H";
		case 0x6D:
			return "i_LD_L_L";
		case 0x6E:
			return "i_LD_L__HL_";
		case 0x6F:
			return "i_LD_L_A";
		case 0x70:
			return "i_LD__HL__B";
		case 0x71:
			return "i_LD__HL__C";
		case 0x72:
			return "i_LD__HL__D";
		case 0x73:
			return "i_LD__HL__E";
		case 0x74:
			return "i_LD__HL__H";
		case 0x75:
			return "i_LD__HL__L";
		case 0x76:
			return "i_HALT";
		case 0x77:
			return "i_LD__HL__A";
		case 0x78:
			return "i_LD_A_B";
		case 0x79:
			return "i_LD_A_C";
		case 0x7A:
			return "i_LD_A_D";
		case 0x7B:
			return "i_LD_A_E";
		case 0x7C:
			return "i_LD_A_H";
		case 0x7D:
			return "i_LD_A_L";
		case 0x7E:
			return "i_LD_A__HL_";
		case 0x7F:
			return "i_LD_A_A";
		case 0x80:
			return "i_ADD_A_B";
		case 0x81:
			return "i_ADD_A_C";
		case 0x82:
			return "i_ADD_A_D";
		case 0x83:
			return "i_ADD_A_E";
		case 0x84:
			return "i_ADD_A_H";
		case 0x85:
			return "i_ADD_A_L";
		case 0x86:
			return "i_ADD_A__HL_";
		case 0x87:
			return "i_ADD_A_A";
		case 0x88:
			return "i_ADC_A_B";
		case 0x89:
			return "i_ADC_A_C";
		case 0x8A:
			return "i_ADC_A_D";
		case 0x8B:
			return "i_ADC_A_E";
		case 0x8C:
			return "i_ADC_A_H";
		case 0x8D:
			return "i_ADC_A_L";
		case 0x8E:
			return "i_ADC_A__HL_";
		case 0x8F:
			return "i_ADC_A_A";
		case 0x90:
			return "i_SUB_A_B";
		case 0x91:
			return "i_SUB_A_C";
		case 0x92:
			return "i_SUB_A_D";
		case 0x93:
			return "i_SUB_A_E";
		case 0x94:
			return "i_SUB_A_H";
		case 0x95:
			return "i_SUB_A_L";
		case 0x96:
			return "i_SUB_A__HL_";
		case 0x97:
			return "i_SUB_A_A";
		case 0x98:
			return "i_SBC_A_B";
		case 0x99:
			return "i_SBC_A_C";
		case 0x9A:
			return "i_SBC_A_D";
		case 0x9B:
			return "i_SBC_A_E";
		case 0x9C:
			return "i_SBC_A_H";
		case 0x9D:
			return "i_SBC_A_L";
		case 0x9E:
			return "i_SBC_A__HL_";
		case 0x9F:
			return "i_SBC_A_A";
		case 0xA0:
			return "i_AND_B";
		case 0xA1:
			return "i_AND_C";
		case 0xA2:
			return "i_AND_D";
		case 0xA3:
			return "i_AND_E";
		case 0xA4:
			return "i_AND_H";
		case 0xA5:
			return "i_AND_L";
		case 0xA6:
			return "i_AND__HL_";
		case 0xA7:
			return "i_AND_A";
		case 0xA8:
			return "i_XOR_B";
		case 0xA9:
			return "i_XOR_C";
		case 0xAA:
			return "i_XOR_D";
		case 0xAB:
			return "i_XOR_E";
		case 0xAC:
			return "i_XOR_H";
		case 0xAD:
			return "i_XOR_L";
		case 0xAE:
			return "i_XOR__HL_";
		case 0xAF:
			return "i_XOR_A";
		case 0xB0:
			return "i_OR_B";
		case 0xB1:
			return "i_OR_C";
		case 0xB2:
			return "i_OR_D";
		case 0xB3:
			return "i_OR_E";
		case 0xB4:
			return "i_OR_H";
		case 0xB5:
			return "i_OR_L";
		case 0xB6:
			return "i_OR__HL_";
		case 0xB7:
			return "i_OR_A";
		case 0xB8:
			return "i_CP_B";
		case 0xB9:
			return "i_CP_C";
		case 0xBA:
			return "i_CP_D";
		case 0xBB:
			return "i_CP_E";
		case 0xBC:
			return "i_CP_H";
		case 0xBD:
			return "i_CP_L";
		case 0xBE:
			return "i_CP__HL_";
		case 0xBF:
			return "i_CP_A";
		case 0xC0:
			return "i_RET_NZ";
		case 0xC1:
			return "i_POP_BC";
		case 0xC2:
			return "i_JP_NZ_nn";
		case 0xC3:
			return "i_JP_nn";
		case 0xC4:
			return "i_CALL_NZ_nn";
		case 0xC5:
			return "i_PUSH_BC";
		case 0xC6:
			return "i_ADD_A_n";
		case 0xC7:
			return "i_RST_0";
		case 0xC8:
			return "i_RET_Z";
		case 0xC9:
			return "i_RET";
		case 0xCA:
			return "i_JP_Z_nn";
		case 0xCB:
			return "i_Ext_ops";
		case 0xCC:
			return "i_CALL_Z_nn";
		case 0xCD:
			return "i_CALL_nn";
		case 0xCE:
			return "i_ADC_A_n";
		case 0xCF:
			return "i_RST_8";
		case 0xD0:
			return "i_RET_NC";
		case 0xD1:
			return "i_POP_DE";
		case 0xD2:
			return "i_JP_NC_nn";
		case 0xD3:
			return "i_XX";
		case 0xD4:
			return "i_CALL_NC_nn";
		case 0xD5:
			return "i_PUSH_DE";
		case 0xD6:
			return "i_SUB_A_n";
		case 0xD7:
			return "i_RST_10";
		case 0xD8:
			return "i_RET_C";
		case 0xD9:
			return "i_RETI";
		case 0xDA:
			return "i_JP_C_nn";
		case 0xDB:
			return "i_XX";
		case 0xDC:
			return "i_CALL_C_nn";
		case 0xDD:
			return "i_XX";
		case 0xDE:
			return "i_SBC_A_n";
		case 0xDF:
			return "i_RST_18";
		case 0xE0:
			return "i_LDH__n__A";
		case 0xE1:
			return "i_POP_HL";
		case 0xE2:
			return "i_LDH__C__A";
		case 0xE3:
			return "i_XX";
		case 0xE4:
			return "i_XX";
		case 0xE5:
			return "i_PUSH_HL";
		case 0xE6:
			return "i_AND_n";
		case 0xE7:
			return "i_RST_20";
		case 0xE8:
			return "i_ADD_SP_d";
		case 0xE9:
			return "i_JP__HL_";
		case 0xEA:
			return "i_LD__nn__A";
		case 0xEB:
			return "i_XX";
		case 0xEC:
			return "i_XX";
		case 0xED:
			return "i_XX";
		case 0xEE:
			return "i_XOR_n";
		case 0xEF:
			return "i_RST_28";
		case 0xF0:
			return "i_LDH_A__n_";
		case 0xF1:
			return "i_POP_AF";
		case 0xF2:
			return "i_XX";
		case 0xF3:
			return "i_DI";
		case 0xF4:
			return "i_XX";
		case 0xF5:
			return "i_PUSH_AF";
		case 0xF6:
			return "i_OR_n";
		case 0xF7:
			return "i_RST_30";
		case 0xF8:
			return "i_LDHL_SP_d";
		case 0xF9:
			return "i_LD_SP_HL";
		case 0xFA:
			return "i_LD_A__nn_";
		case 0xFB:
			return "i_EI";
		case 0xFC:
			return "i_XX";
		case 0xFD:
			return "i_XX";
		case 0xFE:
			return "i_CP_n";
		case 0xFF:
			return "i_RST_38";

	}

	}
}

