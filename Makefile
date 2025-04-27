all:
	$(MAKE) -C src
	$(MAKE) -C tools

CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = 

clean:
	$(RM) out/fanwor
	$(MAKE) -C src clean
	$(MAKE) -C tools clean

.PHONY: all clean
