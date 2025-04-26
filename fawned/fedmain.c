/* ******* fedmain.c - Main routines ******* */

#include <SDL.h>

#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>

#include "feddefs.h"
#include "fwdefs.h"
#include "fedinit.h"
#include "fedgraf.h"
#include "fedkeys.h"
#include "feddisk.h"
#include "level.h"
#include "fwdata.h"


/* **Variables:** */
int mousex, mousey, mousek;        /* For mouse query */
int key;                        /* For keyboard query */

int difgndnum;

SDL_Surface *sdlscrn;
SDL_Surface *offscrn;
SDL_Surface *groundgfx;
SDL_Surface *spritegfx;
SDL_Surface *fontgfx;


int endeflag=FALSE;                     /* TRUE=Exit game */

unsigned char sfield[128][128];          /* The game field */
unsigned char ffield[128][128];
short rwx=0, rwy=0;                     /* Start window at this grid square */
unsigned short rww=16, rwh=11;          /* Current window size (in grid squares) */
unsigned short r_width=16, r_height=11; /* Room dimensions */

unsigned char acttile=0;		/* Selected ground type */
short smode=0;				/* 0=Edit field; 1=Select type */
short tmode=1;

int entry_count=0;                           /* Number of entries */
LEVEL_ENTRY entries[256];                  /* Sprites, etc. */


void printhelp(void)
{
	fprintf(stderr,"\nKeyboard commands:\n");
	fprintf(stderr," n : Clear current field.\n");
	fprintf(stderr," l : Load level.\n");
	fprintf(stderr," s : Save level\n");
	fprintf(stderr," 0 : Sprite-Delete Mode\n");
	fprintf(stderr," 1 : Ground-Set Mode\n");
	fprintf(stderr," 2 : Sprite-Set Mode\n");
	fprintf(stderr," 3 : Door-Set Mode (not working)\n");
	fprintf(stderr," 4 : Item-Set Mode\n");
	fprintf(stderr," d : Show level data (useless)\n");
	fprintf(stderr," h : Show this help\n");
	fprintf(stderr," q : Exit program\n");
}


/* ***Main routine*** */
int main(int argc, char *argv[])
{
	short dx,dy;		/* Dummy variables */
	//short dw,dh;
	SDL_Event event;

	setsprttabl();

	if (initGUI() != 0)
		return -1;

	if (initgraf() != 0)
	{
		fprintf(stderr, "Failed to init graphics\n");
		exitGUI();
		return -1;
	}

	/* Clear the playground */
	for (dy = 0; dy < 128; dy++)
	{
		for (dx=0; dx < 128; dx++)
		{
			sfield[dx][dy]=1;
			ffield[dx][dy]=0;
		}
	}

	fprintf(stderr,"\n    **** Welcome to FAWNED ****\n");

	if (argc == 2 && argv[1][0] != '-')
	{
		loadlevel(argv[1]);
	}

	drawoffscr(rwx, rwy, rww, rwh);
	drwindow();

	/* Change current directory to the room folder */
	if (chdir("../rooms/") != 0)
	{
		fprintf(stderr, "Can not change to rooms directory\n");
		return -1;
	}

	printhelp();

	/* Main loop: */
	do
	{
		if( !SDL_PollEvent(&event) )  continue;

		mousek=SDL_GetMouseState(&mousex, &mousey);
		if (smode == 0)
		{
			unsigned short destx=mousex/32+rwx, desty=mousey/32+rwy;
			fprintf(stderr,"\rX=%i Y=%i  ",destx, desty);
			if (mousek == SDL_BUTTON(1))  /* Clicked on the game field? */
			{
				//SDL_Rect drc;
				int f, x,y;
				switch(tmode)
				{
				case 0:
					for( f=0; f<entry_count; f++ )
						if( entries[f].xpos==destx && entries[f].ypos==desty
						                && (entries[f].entry_type==1 || entries[f].entry_type==2 || entries[f].entry_type==4) )
						{
							x=entries[f].xpos;
							y=entries[f].ypos;
							if( f != entry_count-1 )
								memcpy(&entries[f], &entries[entry_count-1], sizeof(LEVEL_ENTRY));
							--entry_count;
							drawoffscr(x,y,1,1);
							/*drc.x=(x-rwx)*32+wi.g_x;
							drc.y=(y-rwy)*32+wi.g_y;
							drc.w=drc.g_h=32;*/
							drwindow();
						}
					break;
				case 1:
					sfield[destx][desty]=acttile;
					if(acttile>=20) ffield[destx][desty]|=FWALL;
					else ffield[destx][desty]&=~FWALL;
					drawoffscr(destx, desty, 1, 1);
					/*drc.g_x=((mousex-wi.g_x)&0xFFE0)+wi.g_x;
					drc.g_y=((mousey-wi.g_y)&0xFFE0)+wi.g_y;
					drc.g_w=drc.g_h=32;*/
					drwindow();
					break;
				case 2:                         /* Sprite */
					x=0;
					for( f=0; f<entry_count; f++ )
						if( entries[f].xpos==destx && entries[f].ypos==desty) x=1;
					if(x) break;
					entries[entry_count].entry_type=1;
					entries[entry_count].art=acttile;
					entries[entry_count].xpos=destx;
					entries[entry_count].ypos=desty;
					++entry_count;
					drawoffscr(destx, desty, 1, 1);
					/*drc.g_x=((mousex-wi.g_x)&0xFFE0)+wi.g_x;
					drc.g_y=((mousey-wi.g_y)&0xFFE0)+wi.g_y;
					drc.g_w=drc.g_h=32;*/
					drwindow();
					break;
				case 3:                             /* Door */
#if 0
					x=0;
					for( f=0; f<entry_count; f++ )
						if( entries[f].xpos==destx && entries[f].ypos==desty) x=1;
					if(x) break;
					entries[entry_count].entry_type=2;
					entries[entry_count].xpos=destx;
					entries[entry_count].ypos=desty;
					itoa(destx, doordlg[DXPOS].ob_spec.free_string, 10);
					itoa(desty, doordlg[DYPOS].ob_spec.free_string, 10);
					form_center(doordlg, &dx, &dy, &dw, &dh);
					form_dial(FMD_START, dx, dy, 0, 0, dx, dy, dw, dh);
					objc_draw(doordlg, 0, 8, dx, dy, dw, dh);
					form_do(doordlg, 0);
					form_dial(FMD_FINISH, dx, dy, 0, 0, dx, dy, dw, dh);
					doordlg[DOKAY].ob_state=NORMAL;
					entries[entry_count].art=atoi(doordlg[DTYPE].ob_spec.tedinfo->te_ptext);
					dx=atoi(doordlg[DROOMX].ob_spec.tedinfo->te_ptext);
					dy=atoi(doordlg[DROOMY].ob_spec.tedinfo->te_ptext);
					if(dx>=16 || dy>=16)
					{
						form_alert(1,"[3][Error: Target room-|coordinates too|large (max 16).][Abort]");
						break;
					}
					entries[entry_count].spec1=(dx<<4)|dy;
					entries[entry_count].xl=atoi(doordlg[DDXPOS].ob_spec.tedinfo->te_ptext);
					entries[entry_count].yl=atoi(doordlg[DDYPOS].ob_spec.tedinfo->te_ptext);
					++entry_count;
					drc.g_x=((mousex-wi.g_x)&0xFFE0)+wi.g_x;
					drc.g_y=((mousey-wi.g_y)&0xFFE0)+wi.g_y;
					drc.g_w=drc.g_h=32;
#endif
					drwindow();
					break;
				case 4:                         /* Item */
					x=0;
					for( f=0; f<entry_count; f++ )
						if( entries[f].xpos==destx && entries[f].ypos==desty) x=1;
					if(x) break;
					entries[entry_count].entry_type=4;
					entries[entry_count].art=acttile;
					entries[entry_count].xpos=destx;
					entries[entry_count].ypos=desty;
					++entry_count;
					drawoffscr(destx, desty, 1, 1);
					/*drc.g_x=((mousex-wi.g_x)&0xFFE0)+wi.g_x;
					drc.g_y=((mousey-wi.g_y)&0xFFE0)+wi.g_y;
					drc.g_w=drc.g_h=32;*/
					drwindow();
					break;
				}
			}
		}
		else
		{
			unsigned short desttile;
			desttile=mousey/32*rww+mousex/32;
			if(tmode==1 && desttile>=difgndnum) desttile=0;
			if(tmode==2)
			{
				if(desttile>=DIFSPRTNUM) desttile=3;
			}
			if(tmode==4)
			{
				if(desttile>=DIFSPRTNUM) desttile=0;
			}
			fprintf(stderr,"\rTyp=%i  ",desttile);
			if(mousek==SDL_BUTTON(1))
			{
				acttile=desttile;
				mousek=SDL_BUTTON(3);	/* Change mode (see below) */
			}
		}

		if( (tmode==1 || tmode==2 || tmode==4) && ( (mousek==SDL_BUTTON(3))
		                || (event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_SPACE)) ) 	/* Change mode? */
		{
			smode^=1;
			while( SDL_GetMouseState(&mousex, &mousey) )
			{
				SDL_PumpEvents();
			};
			if(smode==0)
			{
				fprintf(stderr, "\nEdit game field!\n");
			}
			else
			{
				switch(tmode)
				{
				case 1:
					fprintf(stderr, "\nSelect ground type!\n");
					break;
				case 2:
					fprintf(stderr, "\nSelect Sprite!\n");
					break;
				case 4:
					fprintf(stderr, "\nSelect Item!\n");
					break;
				}
			}
			drawoffscr(rwx, rwy, rww, rwh);
			drwindow();
		}

		if(event.type==SDL_QUIT)
			endeflag=TRUE;

		if(event.type==SDL_KEYDOWN)
			handle_keypress(event.key.keysym.sym);

	}
	while( !endeflag );


	exitGUI();

	fprintf(stderr,"\nAnd goodbye!\n");

	return(0);
}
