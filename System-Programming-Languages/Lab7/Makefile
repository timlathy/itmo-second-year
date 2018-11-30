CC=gcc
CFLAGS=-std=c99 -g -pedantic-errors -Wall -Werror

all: lab7

lab7: *.s *.c
	nasm -f elf64 image_sepia_sse.s
	$(CC) $(CFLAGS) -c *.c
	$(CC) -ggdb -o lab7 *.o
	@rm *.o

clean:
	@rm lab7
