.PHONY: clean build


all: signature.o
FILE=harness


signature.o: signature.asm
	nasm -f elf64 $< -o $@


clean:
	rm signature.o

build:
	gcc -fno-stack-protector -z execstack $(FILE).c -o $(FILE)
