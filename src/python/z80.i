/* File : z80.i used for generating language bindings to the cpu, we did this to reuse some unit tests */

%module z80
%{
/* Put headers and other declarations here */
#include "z80.h"

%}

%apply unsigned char { uint8_t }
%apply unsigned short { uint16_t }

%inline %{
uint8_t  getReg(z80 * pz, int i) {
      return pz->registers[i];
}
void    setReg(z80 * pz, int i, uint8_t val) {
      pz->registers[i] = val;
}

uint16_t  getReg16(z80 * pz, int i) {
      return pz->registers16[i];
}
void    setReg16(z80 * pz, int i, uint16_t val) {
      pz->registers16[i] = val;
}

%}




#include "z80.h"
