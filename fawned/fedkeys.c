/* **** fed_aes.c: React to AES events, key presses etc. **** */

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "fedmain.h"
#include "feddefs.h"
#include "fedgraf.h"
#include "feddisk.h"
#include "sdlgui.h"

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif




/* ***React to keys*** */
void handle_keypress(int key)
{
	int dx, dy;
	char *new_fname;

	switch(key)
	{
	case 'n':               /* NEWFILE */
		entry_count=0;
		for(dy=0; dy<128; dy++)
			for(dx=0; dx<128; dx++)
			{
				sfield[dx][dy]=1;
				ffield[dx][dy]=0;
			}
		drawoffscr(rwx, rwy, rww, rwh);
		drwindow();
		break;
	case 'l':                 /* LOADFILE */
		new_fname = SDLGui_FileSelect(fname, NULL, 0);
		if (new_fname != NULL)
		{
			strcpy(fname, new_fname);
			free(new_fname);
			loadlevel(fname);
		}
		drawoffscr(rwx, rwy, rww, rwh);
		drwindow();
		break;
	case 's':                 /* SAVEFILE: */
		new_fname = SDLGui_FileSelect(fname, NULL, 1);
		if (new_fname != NULL)
		{
			strcpy(fname, new_fname);
			free(new_fname);
			savelevel(fname);
		}
		break;
	case '0':                 /* ERASE: */
		fprintf(stderr,"\nErase mode\n");
		tmode=0;
		acttile=0;
		break;
	case '1':                /* LVBACKG: */
		fprintf(stderr,"\nBackground setting mode\n");
		tmode=1;
		acttile=0;
		break;
	case '2':                 /* LEVEL_CONTENT: */
		fprintf(stderr,"\nSprite setting mode\n");
		tmode=2;
		acttile=0;
		break;
	case '3':                 /* LVDOORS: */
		fprintf(stderr,"\nDoor setting mode\n");
		tmode=3;
		break;
	case '4':                   /* LVITEMS: */
		fprintf(stderr,"\nItem setting mode\n");
		tmode=4;
		acttile=0;
		break;
	case '5':                 /* LVPEOPLE: */
		fprintf(stderr,"\n(Unsupported) People setting mode\n");
		tmode=5;
		acttile=0;
		break;
	case 'd':                 /* LEVEL DATA: */
		fprintf(stderr,"\nLevel Data:\n");
		fprintf(stderr,"Width: %i  -  Height: %i\n",r_width,r_height);
		rwx=rwy=0;
		drawoffscr(rwx, rwy, rww, rwh);
		drwindow();
		break;
	case 'h':                 /* Show help */
		printhelp();
		break;
	case 'q':                 /* QUITIT */	/* Quit program */
		endeflag=TRUE;
		break;
	}

}
