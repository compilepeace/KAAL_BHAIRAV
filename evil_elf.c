// Author : Abhinav Thakur
// Email  : compilepeace@gmail.com

// Description : This module contains the miscellaneous routines


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





//	CRITICAL GLOBAL VARIABLES 


Elf64_Addr	entry_point_offset;

Elf64_Off	sht_offset,				// Offset (in file) to the start of Section Header Table
			pht_offset;				// Offset (in file) to the start of Program Header Table

uint16_t	pht_entry_count,		// Number of Entries(headers) in Program Header Table
			sht_entry_count;		// Number of Entries(headers) in Section Header Table
	
uint16_t	pht_entry_size,			// Size of 1 entry(header) in Program Header Table
			sht_entry_size;			// Size of 1 entry(header) in Section Header Table

uint64_t	sht_size,				// Size of SHT = no. of entries * size of 1 entry
			pht_size;				// Size of PHT = no. of entries * size of 1 entry

Elf64_Off   end_offset_of_elf;  	// Offset to end of elf (equivalent to size of file on disk)

uint64_t 	text_section_size;		// Size of .text section in file
Elf64_Off	text_section_offset;	// File offset of .text section



// file_size	is Size on file on disk
// image_size	is Size of file after being loaded into memory
void ElfParser(char *filepath)
{
	uint64_t	file_size;
	void 		*map_address;



	// Open the file
	int fd = open(filepath, O_RDWR);
		if (fd == -1) {
			perror(RED"[-]"RESET"elf.c - Infect() - open():");
			exit(0x60);
		}


	// Get the file_size using lstat()
	struct stat statbuf;
		if (lstat(filepath, &statbuf) != 0) {
			perror(RED"[-]"RESET"elf.c - Infect(), lstat():");
			exit(0x61);
		}
		file_size = statbuf.st_size;


	// Map the file into memory for instrumentation
	map_address = mmap(NULL, file_size, PROT_WRITE, MAP_PRIVATE, fd, 0);
		if (map_address == MAP_FAILED) {
			perror("elf.c - Infect(), mmap():");
			exit(0x62);
		}


	int ret;
	ret = ParseElfHeader( map_address );
		// skip Relocatable/Core type file
		// skip 32bit ELFCLASS binaries
		if (ret == 0x2 || ret == 0x3)	return;					


	// Get the offset and size of .text section (on disk representation)
	ParseSHT( map_address );


	//printf("[DEBUG] .text section found @ offset : 0x%lx, size : 0x%lx\n", text_section_offset, text_section_size);


	ParsePHT( map_address );
	

	printf("Unmapping %s!\n", filepath);
	munmap(map_address, file_size);
}



// Extracting usefull info. from binary (mapped @ map_addr) into global variables
int ParseElfHeader(void *map_addr)
{
	Elf64_Ehdr *binary_header = (Elf64_Ehdr *) map_addr;

	
	// ELF Type : Skip relocatable files and core files
	if ( binary_header->e_type == ET_REL ||
		 binary_header->e_type == ET_CORE ) return 0x2;

	
	// ELF CLASS : skip 32-bit class of binary
	if ( binary_header->e_ident[EI_CLASS] == ELFCLASS32 ) return 0x3;



	// Entry Point : store into global variable
	entry_point_offset = binary_header->e_entry;

	// SHT offset  : store into global variable
	sht_offset = binary_header->e_shoff;
	pht_offset = binary_header->e_phoff;

	pht_entry_count = binary_header->e_phnum;
	sht_entry_count = binary_header->e_shnum;

	pht_entry_size = binary_header->e_phentsize;
	sht_entry_size = binary_header->e_shentsize;	

	// Calculating SHT and PHT sizes = no. of entries(headers) * sizeof each entry(header)
	sht_size = sht_entry_count * sht_entry_size;
	pht_size = pht_entry_count * pht_entry_size;

}



void ParseSHT(void *map_addr)
{
	
	end_offset_of_elf = (sht_offset + sht_size);
	

	// Point shdr (Pointer to iterate over SHT) to the last entry of SHT
	Elf64_Shdr *section_entry = (Elf64_Shdr *) (map_addr +
												end_offset_of_elf -
												sht_entry_size);
	
	// .shstrtab section stores all section names (NULL terminated ASCII strings, back-to-back)
	char *shstrtab_section;



	// Parse the SHT bottom->up ( i.e. from last entry(.shstrtab) upto 2nd entry (after NULL entry)) 
	// This way we'll be able to identify .text section in one parse of SHT.
	int i;
	for ( i=0 ; i < (sht_entry_count-1) ; ++i )
	{
		// Name is in the form of an index of .shstrtab section
		Elf64_Off section_name_offset = section_entry->sh_name;


		// ASSUMPTION : Last entry is always .shstrtab section header !
		// Last entry is usually .shstrtab (through which we'll indirectly get the section names)
		if (i == 0)
		{
			// section_entry->offset tells at what offset is the current section is present in binary
			shstrtab_section = (char *)(map_addr + section_entry->sh_offset);
		}
		
		
		else {
			
			//printf("[DEBUG] Iterating ...: %s\n", (char *)(shstrtab_section + section_name_offset));
			
			char *section_name = (char *) (shstrtab_section + section_name_offset);
			if ( strncmp(".text", section_name, 5) == 0 ) {

				text_section_offset = section_entry->sh_offset;
				text_section_size	= section_entry->sh_size; 
			}
		}	

		
		// Move to the next section entry
		--section_entry;
	}
}



void ParsePHT(void *map_addr)
{
	// Point to first entry in PHT
	Elf64_Phdr *phdr_entry = (Elf64_Phdr *)(map_addr + pht_offset);

	
	// Parse PHT entries
	int i;
	for ( i = 0 ; i < pht_entry_count ; ++i)
	{
		printf("pht filesz : 0x%lx -- memsz : 0x%lx\n", phdr_entry->p_filesz, phdr_entry->p_memsz);
		++phdr_entry;
	} 
}



















