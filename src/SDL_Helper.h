#ifndef __SDLHELPER__
#define __SDLHELPER__
#include <SDL/SDL.h>

extern SDL_Surface *screen;
extern uint8_t pressed_buttons[8];

int init_graphics();
void RenderScreen();
void DrawPixel(int x, int y, int pixel);
void Flip(uint8_t* buffer);
void ProcessInput(int * cont);

#define START_KEYCODE 0x28
#define SELECT_KEYCODE 0xe5
#define A_KEYCODE 0xd 
#define B_KEYCODE 0xe
#define UP_KEYCODE 0x1a
#define DOWN_KEYCODE 0x16
#define LEFT_KEYCODE 0x04
#define RIGHT_KEYCODE 0x07

#define START_KEY 0
#define SELECT_KEY 1
#define A_KEY 2
#define B_KEY 3
#define UP_KEY 4
#define DOWN_KEY 5
#define LEFT_KEY 6
#define RIGHT_KEY 7


#endif