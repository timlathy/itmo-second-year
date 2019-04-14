CC=gcc

all: lab7

lab7: *.asm
	nasm -g -p ../../System-Programming-Languages/Lab1/lib.inc -F dwarf -f elf64 interp.asm
	ld -o forth *.o
	@rm *.o

clean:
	@rm lab7
