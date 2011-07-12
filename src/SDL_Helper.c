#include "SDL_Helper.h"
#include <stdlib.h>

SDL_Surface *screen;
uint8_t pressed_buttons[8];
uint8_t button_irq = 0;

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
		int upordown = 0;
		if(!SDL_PollEvent(&event))
			return;
		switch(event.type){
			case SDL_KEYDOWN:
				upordown = 0;
				button_irq = 1;
				goto keyswitch;
			case SDL_KEYUP:
				upordown = 1;
				keyswitch:
				switch(event.key.keysym.sym)
				{
					case SDLK_j:
						pressed_buttons[A_KEY] = upordown;
						break;
					case SDLK_k:
						pressed_buttons[B_KEY] = upordown;
						break;
					case SDLK_w:
						pressed_buttons[UP_KEY] = upordown;
						break;
					case SDLK_s:
						pressed_buttons[DOWN_KEY] = upordown;
						break;
					case SDLK_a:
						pressed_buttons[LEFT_KEY] = upordown;
						break;
					case SDLK_d:
						pressed_buttons[RIGHT_KEY] = upordown;
						break;
					case SDLK_RETURN:
						pressed_buttons[START_KEY] = upordown;
						break;
					case SDLK_RSHIFT:
						pressed_buttons[SELECT_KEY] = upordown;
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



