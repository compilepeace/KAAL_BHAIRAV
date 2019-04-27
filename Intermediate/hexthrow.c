// Author : Abhinav Thakur
// Email  : compilepeace@gmail.com

// Description : This program takes path_to_file as a command line argument and returns a hexdump of
//				 it. Produces space character in all the places where '\n' or '\t' occurs to 
//				 maintain allignment.


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#include <fcntl.h>
#include <sys/types.h>

#include "color.h"

#define MAXSIZE 16


int main(int argc, char *argv[])
{
	uint8_t buffer[MAXSIZE];
	uint32_t offset = 0x0, bytes_read = 0x0;
	uint32_t i;
	size_t fd;


	// Verifying the command line args
	if (argc < 2)
	{
		fprintf(stderr, BOLDRED"[-] Usage:" RESET " %s <filename> \n"RESET, argv[0]);
		exit(0x10);
	}


	fprintf(stdout, BOLDGREEN"\n\n[+] Command line arguments verified.\n"RESET);


	// Open the file with Read-Only permissions
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, BOLDRED"[-] Error:" RESET " While opening '%s' \n"RESET, argv[1]);
		exit(0x11); 
	}
	
	
	fprintf(stdout, BOLDGREEN"[+] %s opened as Read-Only. \n\n" RESET, argv[1]);
	

	// read 16 bytes at a time and increment offset
	while( (bytes_read = read(fd, buffer, sizeof buffer)) )
	{	

		fprintf(stdout, BOLDRED"0x%08x: " RESET, offset);

	
		// Print hexadecimal representation of each character in buffer
		for ( i = 0x0; i < sizeof buffer ; ++i)
			fprintf(stdout, GREEN"%02x " RESET, *(buffer + i) );
	
		
		// Print out each character also after hexadecimal representation
		fprintf(stdout, "\t| ");

		for ( i = 0x0; i < sizeof buffer ; ++i)
		{	
			// If character is '\n' or '\t', print a blank space
			if ( *(buffer + i) == 0x0a || *(buffer + i) == 0x9)
			{
				fprintf(stdout, " ");
				continue;
			}
			
			// otherwise print the character as it is
			fprintf(stdout, YELLOW "%c" RESET, *(buffer + i));
		}

		fprintf(stdout, " |");	


		// Increment offset by 0x10 bytes
		offset += 0x10;
		fprintf(stdout, "\n");


	} 	


	fprintf(stdout, "\n");
	close(fd);

return 0;
}
