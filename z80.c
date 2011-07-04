#include "z80.h"
#include "mmu.h"

#include "strings.h" //bzero


uint8_t 
buildStatusFlag(int zero, int sub, int halfcarry,int carry){
	uint8_t ret = 0;
	if(zero)
		ret = setFlag(ret,ZERO);
	if(sub)
		ret = setFlag(ret,SUB);
	if(halfcarry)
		ret = setFlag(ret,HALFCARRY);
	if(carry)
		ret = setFlag(ret,CARRY);
	return ret;
}





void 
initZ80(z80*z){
	bzero((void*)z,sizeof(z80));
}
