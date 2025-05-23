/*************************************************************/
/* fwguiini.c  -  Graphical User Interface init routines     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Written and (c) by Thomas Huth                            */
/* See "fwsource.txt" for more information                   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* This file is distributed under the GNU General Public     */
/* License, version 2 or at your option any later version.   */
/* Please read the file "gpl.txt" for details.               */
/*************************************************************/

#include <string.h>

#include <SDL.h>

#include "fwgui.h"
#include "fwgraf.h"
#include "fwmain.h"
#include "loadpng.h"
#include "fwguiini.h"

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif


/* Variables: */
static const char *spritename = "./graphics/sprites.png";
static const char *groundname = "./graphics/ground.png";
static const char *fontname = "./graphics/font.png";


/* *** Login to GEM + Keyboard/Joystick initialize *** */
int initGUI(void)
{
	int i, videoflags;

	if( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0 )
	{
		fprintf(stderr, "Could not initialize the SDL library:\n %s\n", SDL_GetError() );
		return 1;
	}

	videoflags=SDL_SWSURFACE|SDL_HWPALETTE;
	for(i=0; i<g_argc; i++)
	{
		if( strcmp(g_argv[i], "-f")==0 )
		{
			videoflags|=SDL_FULLSCREEN;
			break;
		}
	}
	sdlscrn=SDL_SetVideoMode(512,384,0,videoflags);
	if( sdlscrn==NULL )
	{
		fprintf(stderr, "Could not set video mode:\n %s\n", SDL_GetError() );
		return 2;
	}

	SDL_FillRect(sdlscrn,NULL, SDL_MapRGB(sdlscrn->format, 0, 0, 0) );

	SDL_WM_SetCaption("Fanwor - The Legend of Gemda", "Fanwor");

	/* Ignore unused events: */
	SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
	SDL_ShowCursor(0);

	return 0;
}


/* ***Logout from SDL*** */
void exitGUI(void)
{
	SDL_Quit();
}



/* ***Open the main window (not used for SDL)*** */
int open_window(void)
{
	return 0;
}


/* ***Close the window*** */
void close_window(void)
{
}



/* ***Load and prepare graphics*** */
long initgraf(void)
{
	/* Offscreen Surface */
	offscrn=SDL_DisplayFormat(sdlscrn);
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

	/* Load font gfx: */
	fontgfx=LoadPNG(fontname);
	if( fontgfx==NULL )
	{
		fprintf(stderr, "Could not load image:\n %s\n", SDL_GetError() );
		return 3;
	}

	return 0;
}


/* ***Free the gfx from the memory*** */
void deinitgraf(void)
{
	SDL_FreeSurface(offscrn);
	SDL_FreeSurface(spritegfx);
	SDL_FreeSurface(groundgfx);
}
