/* File : z80.i used for generating language bindings to the cpu, we did this to reuse some unit tests */

%module z80
%{
/* Put headers and other declarations here */
#include "z80.h"

%}

%apply unsigned char { uint8_t }
%apply unsigned short { uint16_t }

%inline %{
uint8_t  getReg(uint8_t * arr, int i) {
      return arr[i];
}
void    setReg(uint8_t * arr, int i, uint8_t val) {
      arr[i] = val;
}

uint16_t  getReg16(uint16_t * arr, int i) {
      return arr[i];
}
void    setReg16(uint16_t * arr, int i, uint16_t val) {
      arr[i] = val;
}

%}




#include "z80.h"
