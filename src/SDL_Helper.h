#ifndef __SDLHELPER__
#define __SDLHELPER__
#include <SDL/SDL.h>

extern SDL_Surface *screen;
extern uint8_t pressed_buttons[8];
extern uint8_t button_irq;
extern uint8_t saveState;

int init_graphics();
void RenderScreen();
void DrawPixel(int x, int y, int pixel);
void Flip(uint8_t* buffer);
void ProcessInput(int * cont);
uint8_t GetPixelVal(int pixel);

uint32_t get_timer_val(int reset);


#define START_KEY 0
#define SELECT_KEY 1
#define A_KEY 2
#define B_KEY 3
#define UP_KEY 4
#define DOWN_KEY 5
#define LEFT_KEY 6
#define RIGHT_KEY 7


#endif
