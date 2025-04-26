/* ****** fedmain.h ****** */

#include <SDL.h>

#include "level.h"

extern int mousex, mousey, mousek, clicks;
extern int kstate, key;
extern short rwx, rwy;
extern unsigned short rww, rwh, r_width, r_height;
extern unsigned short newpal[256][3];
extern int endeflag;
extern unsigned char sfield[128][128], ffield[128][128];
extern unsigned char acttile;
extern short smode;
extern short tmode;
extern LEVEL_ENTRY entries[];
extern int entry_count;

extern int difgndnum;

extern char flag_fullscreen;

extern SDL_Surface *sdlscrn;
extern SDL_Surface *offscrn;
extern SDL_Surface *groundgfx;
extern SDL_Surface *spritegfx;
extern SDL_Surface *fontgfx;

extern void printhelp(void);
