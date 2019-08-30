
.PHONY: clean 

CC= gcc

OBJECTS= objects
SPIDER= bin/spider 
 
FLAGS= --std=gnu99

all: $(SPIDER)



$(SPIDER): file_tamper.o dir_tamper.o misc_routines.o evil_elf.o main.o
	mv *.o objects
	$(CC) $(OBJECTS)/file_tamper.o $(OBJECTS)/dir_tamper.o $(OBJECTS)/main.o $(OBJECTS)/misc_routines.o $(OBJECTS)/evil_elf.o -g -o $(SPIDER)


$(OBJECTS)/main.o: main.c
	$(CC) -c $< -o -g $(OBJECTS)


$(OBJECTS)/file_tamper.o: file_tamper.c
	$(CC) -c $< -o -g $(OBJECTS)


$(OBJECTS)/dir_tamper.o: dir_tamper.c
	$(CC) -c $< -o -g $(OBJECTS)


$(OBJECTS)/misc_routines.o: misc_routines.c
	$(CC) -c $< -o -g $(OBJECTS)


$(OBJECTS)/evil_elf.o: evil_elf.c
	$(CC) -c $< -o -g $(OBJECTS)


clean: 
	rm $(SPIDER) $(OBJECTS)/*

