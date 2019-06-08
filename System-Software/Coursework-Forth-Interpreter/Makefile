CC=gcc

all: lab7

lab7: *.asm
	nasm -g -F dwarf -f elf64 main.asm
	ld -o forth *.o
	@rm *.o

clean:
	@rm lab7
