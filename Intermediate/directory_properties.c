// Author : Abhinav Thakur
// Email  : compilepeace@gmail.com

// Description : The program traverses through an open directory and prints out the type of the file
//				 in the directory listing 

/*
		  struct dirent {
               ino_t          d_ino;       // Inode number 
               off_t          d_off;       // Not an offset; see below 
               unsigned short d_reclen;    // Length of this record 
               unsigned char  d_type;      // Type of file; not supported
                                           //   by all filesystem types 
               char           d_name[256]; // Null-terminated filename 
   		  };
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <assert.h>

#include "color.h"

#define SIZE 500
#define TYPESIZE 20


const char *get_entry_type(const char *);


int main(int argc, char *argv[])
{
	char current_dir[SIZE], entry_name[SIZE+256];
	DIR *dir_handle;
	struct dirent *dir_entry;
	const char *filetype;
	int sizeof_arg;


	// Verify command line arguments and set the current working directory
	if (argc < 2)
	{
		getcwd(current_dir, SIZE);

		// exit if getcwd returns NULL or if it returns "(unreachable"
		if ( *current_dir == '(' || current_dir == NULL )
		{
			fprintf(stderr, RED"[-] getcwd:" RESET" returned %s \n", current_dir);
			exit(0x10);
		}
	
		// Appending a '/' at the end of current directory path 
		strncat(current_dir, "/", 1);	

		fprintf(stdout, GREEN"[+]" RESET" Usage: %s <path_to_directory> \n", argv[0]);
		fprintf(stdout, GREEN"[+]" RESET" Setting the working directory to"MAGENTA" %s \n"RESET, current_dir); 
	}


	// Generate the path and end with a NULL character
	else
	{
		sizeof_arg = strlen(argv[1]);
		strncpy(current_dir, argv[1], sizeof_arg);
		current_dir[sizeof_arg] = '\x00';			
		
		// Check the last character and place a '/' if the last character of argv[1] is not '/'
		if (current_dir[sizeof_arg - 1] != '/')
			strncat(current_dir, "/", 1);	
			
		fprintf(stdout, GREEN"[+] Setting current working directory to %s \n", current_dir);
	}


	// open up the directory
	dir_handle = opendir(current_dir);
	if (dir_handle == NULL)
	{
		fprintf(stderr, RED"opendir:" RESET" could not open directory stream -> %s \n", current_dir);
		exit(0x11);
	}

	fprintf(stdout, "\n\n");


	// go through the directory entries 	
	while ((dir_entry = readdir(dir_handle)))
	{
		memmove(entry_name, "0", sizeof entry_name);
		strncpy(entry_name, current_dir, strlen(current_dir));
		strncat(entry_name, dir_entry->d_name, strlen(dir_entry->d_name));
		filetype = get_entry_type( entry_name );
		fprintf(stdout, RED"%-20s"RESET ": "GREEN " %s\n", filetype, entry_name);
	}

	// closing the directory handle
	closedir(dir_handle);	
	
	fprintf(stdout,"\n\n");

return 0;
}


// Returns the type of file
const char *get_entry_type(const char *path)
{	
	struct stat statbuf;

	lstat(path, &statbuf);

	
	if ( S_ISBLK(statbuf.st_mode) )
		return "block device";

	else if ( S_ISCHR(statbuf.st_mode) )
		return "character device";
	
	else if ( S_ISDIR(statbuf.st_mode) )
		return "directory";

	else if ( S_ISFIFO(statbuf.st_mode) )
		return "FIFO/pipe";

	else if ( S_ISLNK(statbuf.st_mode) )
		return "symlink";

	else if ( S_ISREG(statbuf.st_mode) )
		return "regular file";

	else if ( S_ISSOCK(statbuf.st_mode) )
		return "socket";

	else
		// Unknown filetype
		return "unknown";
}

