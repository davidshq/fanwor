# ***Makefile for Fanwor***

# Select your architecture:
# - sdl   : Linux/Unix SDL Library
# - atari : Atari GEM
ARCH=sdl

# If you don't need/want music on Linux with the SDL-Mixer lib,
# you can comment out the following two lines:
SOUNDLIB = -lSDL_mixer
SOUNDCFL = -DWITH_SOUND

# What io library do we need? Perhaps you also have to edit this:
LIBS_sdl = -lpng $(shell sdl-config --libs) -lz $(SOUNDLIB)
LIBS_atari = -laes -lvdi

# System specific CFLAGS:
CFL_sdl = $(shell sdl-config --cflags) $(SOUNDCFL) -g -O2 \
	  -Wall -Wmissing-prototypes -Wstrict-prototypes
CFL_atari = 


ARCHDIR=./arch/$(ARCH)
OUTDIR=../out/src
OUT_ARCHDIR=$(OUTDIR)/arch/$(ARCH)

CFLAGS=-I. -I./arch/$(ARCH) $(CFL_$(ARCH))

LIBS = $(LIBS_$(ARCH))


OBS_MAIN = $(OUTDIR)/fwdisk.o $(OUTDIR)/fwfight.o $(OUTDIR)/fwreact.o $(OUTDIR)/fwmain.o $(OUTDIR)/fwdata.o
OBS_ARCH = $(OUT_ARCHDIR)/fwguiini.o $(OUT_ARCHDIR)/fwgui.o $(OUT_ARCHDIR)/fwgraf.o $(OUT_ARCHDIR)/fwmusic.o
OBS_atari = $(OUT_ARCHDIR)/loadimg.o $(OUT_ARCHDIR)/windial.o $(OUT_ARCHDIR)/fw_asm.o
OBS_sdl = $(OUT_ARCHDIR)/loadpng.o
OBS_XTRA = $(OBS_$(ARCH))


fanwor: $(OBS_MAIN) $(OBS_ARCH) $(OBS_XTRA)
	$(CC) $(OBS_MAIN) $(OBS_ARCH) $(OBS_XTRA) $(LIBS) -o ../out/fanwor

$(OUTDIR)/%.o: %.c
	@mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_ARCHDIR)/%.o: $(ARCHDIR)/%.c
	@mkdir -p $(OUT_ARCHDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR)/fwmain.o: fwdefs.h
$(OUTDIR)/fwreact.o: fwdefs.h
$(OUTDIR)/fwdisc.o: level.h

clean:
	$(RM) $(OBS_MAIN) $(OBS_ARCH) $(OBS_XTRA) *.bak *~
	$(RM) -r $(OUTDIR)
