CC=gcc
CFLAGS=-std=c11 -g -Wall -Werror -Ofast -march=native

all: lab7

lab7: *.asm *.c
	nasm -g -F dwarf -f elf64 image_sepia_sse.asm
	nasm -g -F dwarf -f elf64 image_sepia_avx.asm
	$(CC) $(CFLAGS) -c *.c
	$(CC) -ggdb -o lab7 *.o
	@rm *.o

clean:
	@rm lab7
