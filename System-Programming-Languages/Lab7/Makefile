CC=gcc
CFLAGS=-std=c99 -g -pedantic-errors -Wall -Werror

all: lab7

lab7: *.asm *.c
	nasm -g -F dwarf -f elf64 image_sepia_sse.asm
	$(CC) $(CFLAGS) -c *.c
	$(CC) -ggdb -o lab7 *.o
	@rm *.o

clean:
	@rm lab7
