CC=gcc

all: forth

forth: *.asm
	nasm -g -F dwarf -f elf64 main.asm
	ld -o forth *.o
	@rm *.o

clean:
	@rm forth
