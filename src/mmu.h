#ifndef __mmu__
#define __mmu__
#include "stdint.h"
#include "gpu.h"

/* CONSTANTS */
#define MEMORYSIZE 65536
#define EXT_RAM_SIZE 8192
#define WORKING_RAM_SIZE 8192
#define ZERO_PAGE_RAM_SIZE 128
#define BIOS_SIZE 256
#define OAM_SIZE 160
#define CARTRIDGE_SIZE 300000 //32768 TODO: dynamically resize to rom size

#define __BIOS__ {0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,\
  0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0,\
  0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,\
  0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,\
  0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,\
  0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,\
  0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,\
  0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,\
  0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xF2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,\
  0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,\
  0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,\
  0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,\
  0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,\
  0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3c, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x4C, \
  0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20, 0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x20,\
  0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x20, 0xFE, 0x3E, 0x01, 0xE0, 0x50}

#define MBC1_16_8_MODE 0 
#define MBC1_4_32_MODE 1

#define ROM_ONLY 0
#define MBC1 1

/* Data types */
//#define USE_ADDRESS_LUT
typedef struct _mmu {
	uint8_t bios_enabled;
	uint8_t memory[MEMORYSIZE];
	uint8_t wram[WORKING_RAM_SIZE];
	uint8_t zram[ZERO_PAGE_RAM_SIZE];
	uint8_t eram[EXT_RAM_SIZE];
	uint8_t oam[OAM_SIZE];
	uint8_t bios[BIOS_SIZE];
	uint8_t cartridge[CARTRIDGE_SIZE];
	uint8_t rom_type;
	uint8_t mbc1_mode;
	uint8_t rom_bank;
	uint8_t ram_bank;
	uint8_t ram_bank_enable;
	GPU gpu;
	#ifdef USE_ADDRESS_LUT
	uint8_t * addressLUT[MEMORYSIZE];
	#endif
} MMU;

#ifdef USE_ADDRESS_LUT
void
disableBios(MMU*pmmu);
#endif

/* functions */

void initMMU(MMU * pmmu);

uint8_t rb(MMU * pmmu,uint16_t address);
uint16_t rw(MMU * pmmu,uint16_t address);

void wb(MMU * pmmu,uint16_t address, uint8_t val);
void ww(MMU * pmmu,uint16_t address, uint16_t val);

//#define DEBUG

#ifdef DEBUG
extern int logging_enabled;
#endif
#endif
