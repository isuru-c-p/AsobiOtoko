#include "SDL_Helper.h"
#include <stdlib.h>

SDL_Surface *screen;
uint8_t pressed_buttons[8];

int init_graphics()
{
	if( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0) {
		printf("Unable to init SDL: %s\n", SDL_GetError());
		return 0;
	}
	
	screen = SDL_SetVideoMode(160, 144, 8, SDL_HWSURFACE);
	if ( screen == NULL ) {
        printf("Unable to set 160x144 video: %s\n", SDL_GetError());
        return 0;
    }
	
	int i;
	for(i = 0; i < 8; i++)
		pressed_buttons[i] = 1;
	
	return 1;
}

uint8_t GetPixelVal(int pixel)
{	
	return (uint8_t)SDL_MapRGB(screen->format, pixel, pixel, pixel);
}

void DrawPixel(int x, int y, int pixel)
{
	Uint32 color = SDL_MapRGB(screen->format, pixel, pixel, pixel);
	Uint8 *bufp;
	bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
	*bufp = color;
}

void Flip(uint8_t* buffer)
{
	memcpy((Uint8 *)screen->pixels, buffer, 160*144);
	return;
}

void RenderScreen()
{
	SDL_UpdateRect(screen, 0, 0, 0, 0);	
}

void ProcessInput(int * cont){
		static	SDL_Event event;
		SDL_PollEvent(&event);
		switch(event.type){
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				switch(event.key.keysym.scancode)
				{
					case A_KEYCODE:
						pressed_buttons[A_KEY] = 0;
						break;
					case B_KEYCODE:
						pressed_buttons[B_KEY] = 0;
						break;
					case UP_KEYCODE:
						pressed_buttons[UP_KEY] = 0;
						break;
					case DOWN_KEYCODE:
						pressed_buttons[DOWN_KEY] = 0;
						break;
					case LEFT_KEYCODE:
						pressed_buttons[LEFT_KEY] = 0;
						break;
					case RIGHT_KEYCODE:
						pressed_buttons[RIGHT_KEY] = 0;
						break;
					case START_KEYCODE:
						pressed_buttons[START_KEY] = 0;
						break;
					case SELECT_KEYCODE:
						pressed_buttons[SELECT_KEY] = 0;
						break;
					default:
						break;
				}
				break;
			case SDL_QUIT:
				*cont = 0;
				break;
		}
}



