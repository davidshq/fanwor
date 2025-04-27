# Tools Makefile

CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = 

all: room_converter

room_converter: room_converter.c
	$(CC) $(CFLAGS) -o room_converter room_converter.c $(LIBS)

clean:
	rm -f room_converter
	
.PHONY: all clean 