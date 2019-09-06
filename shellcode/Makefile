.PHONY: clean build


all: signature.o harness


signature.o: signature.asm
	nasm -f elf64 $< -o $@

harness: harness.c
	gcc -fno-stack-protector -z execstack $< -o $@


clean:
	rm signature.o harness

