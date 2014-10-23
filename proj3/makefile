# Alex Shoop CS 3013 project 3 makefile

LIB=-lpthread -lrt
CC=gcc

all: addem life

addem: addem.c
	 $(CC) addem.c -o addem $(LIB)

life: life.c
	 $(CC) life.c -o life $(LIB)

clean: 
	/bin/rm -f *.o life addem
