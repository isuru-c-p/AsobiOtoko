#ifndef __SDLHELPER__
#define __SDLHELPER__
#include <SDL/SDL.h>

SDL_Surface *screen;
int init_graphics();
void RenderScreen();
void DrawPixel(int x, int y, int pixel);
void Flip(uint8_t* buffer);

#endif