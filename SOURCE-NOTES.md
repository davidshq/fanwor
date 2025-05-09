# Fanwor - Some words on the source code

I've released the complete source code of my game Fanwor including the level
editor Fawned under the terms of the GNU General Public License (please read
the file "gpl.txt" for details).

Please note that I wrote Fanwor while I was still a ~20 years old guy, not
very experienced with programming yet. Most parts of the source code are likely
very ugly, and I certainly would not write it in the same style today.

The source is also not very well documented and sometimes also only in german,
but I hope it is basically understandable.

I used SozobonX-C to compile on the Atari it, but with some minimal work, you
should also be able to use another C compiler (Pure-C, GNU-C, ...) to compile
it.

For the Linux/Unix version, any C compiler like GCC or clang should be usable.
You also need to install some additional libraries with their development
headers first before you can compile Fanwor - please see the readme.txt for
more information.

If you want to write an extension to Fanwor/Fawned, please contact me first so
that I can sync the work on it. And please also keep to its actual source file
scheme: There are platform independent files and platform dependent files.

platform independent files:

 - fwdata.c    (A table with enemies/items/weapons' information - will be put in
                an external file one day...)
 - fwdisk.c    (Disk io routines)
 - fwfight.c   (routines concerning the battle with the enemies)
 - fwmain.c    (the main() function and some other base functions)
 - fwreact.c   (hero movement, test for doors, people (will come one day...),
                items, ...)

 
platform dependent files:

 - fwgraf.c    (graphical output routines)
 - fwgui.c     (graphical user interface routines, menus, dialogs)
 - fwguiini.c  (graphics and GUI initialisation)
 - fwmusic.c   (Music and sound fx)

Atari only platform dependent files:

 - loadimg.c   (some routines to load GEM *.IMG files)
 - windial.c   (routines for window dialogs under GEM)
 - fw_asm.s    (some assembler routines for using the joystick)

Linux only platform dependent files:

 - loadpng.c   (for loading PNG images)

Feel free to introduce new files but please contact me before.
The *.h files mostly contain the prototypes and external variables definitions
to the corresponding *.c files. One exceptions is the fwdefs.h, which contains
some central definitions and declarations.

Some words about Fawned, the Fanwor Editor: It is in a catastrophal beta
version state. Until now, I was to lazy to document it or to debug it.
Please use it very carefully.

And now, have a lot of fun with the source!
