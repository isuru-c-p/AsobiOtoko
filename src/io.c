#include "io.h"
#include "SDL_Helper.h"

uint8_t P1 = 0;


void writeP1(uint8_t val)
{
	//printf("Writing val: %x\n", val);
	P1 = (val & 0x30);
}

uint8_t readP1()
{
	uint8_t val = ((P1 & 0x30) | getButtons());
	//printf("Read P1: %x\n", val);
	return val;
	
}

uint8_t getButtons()
{
	uint8_t retval = 0;

	switch(((P1 & 0x30) >> 4))
	{
		// P14 enabled
		case 2:
			retval = ((pressed_buttons[RIGHT_KEY]) + (pressed_buttons[LEFT_KEY] << 1) + (pressed_buttons[UP_KEY] << 2) + (pressed_buttons[DOWN_KEY] << 3));
			pressed_buttons[RIGHT_KEY] = 1;
			pressed_buttons[LEFT_KEY] = 1;
			pressed_buttons[UP_KEY] = 1;
			pressed_buttons[DOWN_KEY] = 1;
			return retval;
		// P15 enabled
		case 1:
			retval = ((pressed_buttons[A_KEY]) + (pressed_buttons[B_KEY] << 1) + (pressed_buttons[SELECT_KEY] << 2) + (pressed_buttons[START_KEY] << 3));
			pressed_buttons[A_KEY] = 1;
			pressed_buttons[B_KEY] = 1;
			pressed_buttons[START_KEY] = 1;
			pressed_buttons[SELECT_KEY] = 1;
			return retval;
		// anomaly
		default:
			printf("Invalid P14-P15 value: %d\n", ((P1 & 0x30) >> 4));
			return P1;
	}
}