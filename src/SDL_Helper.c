#include "SDL_Helper.h"
#include <stdlib.h>

int init_graphics()
{
	if( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		return -1;
	}
	
	screen = SDL_SetVideoMode(160, 144, 8, SDL_SWSURFACE);
	if ( screen == NULL ) {
        fprintf(stderr, "Unable to set 160x144 video: %s\n", SDL_GetError());
        return -1;
    }
	
	return 0;
}

void DrawPixel(int x, int y, int pixel)
{
	Uint32 color = SDL_MapRGB(screen->format, pixel, pixel, pixel);
	Uint8 *bufp;
	bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
	*bufp = color;
}

void RenderScreen()
{
	SDL_UpdateRect(screen, 0, 0, 0, 0);	
}


