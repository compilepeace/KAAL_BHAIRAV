// Author : Abhinav Thakur
// Email  : compilepeace@gmail.com

// Description : This module contains the miscellaneous routines


#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "color.h"


// Returns a joined string (path + name) 
char *GetAbsPath(char *path, char *name)
{

	char *pathname = (char *)calloc(1000, 1);
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
