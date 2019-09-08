.PHONY: clean build


all: exec_parasite.bin so_parasite.bin harness


exec_parasite.bin: exec_parasite.asm
	nasm -f bin $< -o $@

so_parasite.bin: so_parasite.asm
	nasm -f bin $< -o $@

harness: harness.c
	gcc -fno-stack-protector -z execstack $< -o $@


clean:
	rm parasite_exec.bin parasite_so.bin harness

