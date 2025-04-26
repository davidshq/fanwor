/* ***** fedinit.c: (De-)Initialization of GEM, graphics, etc. ***** */

#include <SDL.h>
#include <string.h>
#include <stdbool.h>

#include "feddefs.h"
#include "fedmain.h"
#include "fedgraf.h"
#include "loadpng.h"
#include "sdlgui.h"


/* **Variables:** */
const char *spritename="../graphics/sprites.png";
const char *groundname="../graphics/ground.png";


/* *** Login to GUI *** */
int initGUI(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Could not initialize the SDL library:\n %s\n", SDL_GetError() );
		return 1;
	}

	sdlscrn=SDL_SetVideoMode(rww*32, (rwh+1)*32, 8, SDL_SWSURFACE|SDL_HWPALETTE);
	if (sdlscrn == NULL)
	{
		fprintf(stderr, "Could not set video mode:\n %s\n", SDL_GetError() );
		return 2;
	}

	SDL_FillRect(sdlscrn,NULL, SDL_MapRGB(sdlscrn->format, 0, 0, 0) );

	SDL_WM_SetCaption("FAWNED - The Fanwor Editor", "Fawned");

	SDL_ShowCursor(1);

	SDLGui_Init();
	SDLGui_SetScreen(sdlscrn);

	return 0;
}


/* ***Logout from GUI*** */
void exitGUI(void)
{
	SDLGui_UnInit();
	SDL_Quit();
}


/* ***Load and prepare graphics*** */
long initgraf(void)
{
	/* Offscreen Surface */
	offscrn=sdlscrn; //SDL_DisplayFormat(sdlscrn);
	if( offscrn==NULL )
	{
		fprintf(stderr, "Could not create offscreen bitmap:\n %s\n", SDL_GetError() );
		return 3;
	}

	/* Load and prepare unit graphics: */
	spritegfx=LoadPNG(spritename);
	if( spritegfx==NULL )
	{
		fprintf(stderr, "Could not load image:\n %s\n", SDL_GetError() );
		return 3;
	}

	/* Set transparent pixel as the pixel at (0,0) */
	if ( spritegfx->format->palette )
		SDL_SetColorKey(spritegfx, (SDL_SRCCOLORKEY|SDL_RLEACCEL), *(Uint8 *)spritegfx->pixels);

	/* Load and prepare ground graphics: */
	groundgfx=LoadPNG(groundname);
	if( groundgfx==NULL )
	{
		fprintf(stderr, "Could not load image:\n %s\n", SDL_GetError() );
		return 3;
	}
	difgndnum=groundgfx->h/32;

	return(0);
}
