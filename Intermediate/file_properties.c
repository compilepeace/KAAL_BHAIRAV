// Author : Abhinav Thakur
// Email  : compilepeace@gmail.com

// Description : This program uses fstat() to determine the size of file (at runtime) and allocate 
//				 big enough buffer for it so that it can store the contents of file into it (concept
//				 of Variable Length Arrays - VLA's) or we can use heaps (dynamic memory allocation
//				 using malloc) and displays the properties of that file.

/*
           struct stat {
               dev_t     st_dev;         // ID of device containing file 
               ino_t     st_ino;         // Inode number 
               mode_t    st_mode;        // File type and mode 
               nlink_t   st_nlink;       // Number of hard links 
               uid_t     st_uid;         // User ID of owner 
               gid_t     st_gid;         // Group ID of owner 
               dev_t     st_rdev;        // Device ID (if special file) 
               off_t     st_size;        // Total size, in bytes 
               blksize_t st_blksize;     // Block size for filesystem I/O 
               blkcnt_t  st_blocks;      // Number of 512B blocks allocated 

               // Since Linux 2.6, the kernel supports nanosecond
               // precision for the following timestamp fields.
               // For the details before Linux 2.6, see NOTES. 

               struct timespec st_atim;  // Time of last access 
               struct timespec st_mtim;  // Time of last modification 
               struct timespec st_ctim;  // Time of last status change 

           #define st_atime st_atim.tv_sec      // Backward compatibility 
           #define st_mtime st_mtim.tv_sec
           #define st_ctime st_ctim.tv_sec
           };
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "color.h"


int main(int argc, char *argv[])
{	
	struct stat stat_buf;
	off_t size = 0;


	// Verifying command line args
	if (argc < 2)
	{
		fprintf(stderr, RED "[-] Usage:" RESET" %s <filename> \n", argv[0]);
		exit(0x10); 
	}	

	// opening file
	int fd = open(argv[1], O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, RED "[-] Error: " RESET "while opening %s \n", argv[0]);
		exit(0x12);
	}

	// Extracting its properties 
	int status = fstat(fd, &stat_buf);
	if (status == -1)
	{	
		fprintf(stderr, RED "[-] Erro: " RESET "while extracting file properties in fstat() \n");
		exit(0x13);
	}


	// Check if its a regular file
	if ( !S_ISREG(stat_buf.st_mode) )
		fprintf(stdout, BOLDMAGENTA"\n\n[+] Not a regular file\n" RESET);
	else
		fprintf(stdout, BOLDMAGENTA"\n\n[+] A Regular file\n"RESET);


	// Displaying important properties of the file
	size = stat_buf.st_size;
	fprintf(stdout, BOLDCYAN "\nProperties of file are : \n" RESET);
	fprintf(stdout, GREEN "\tSize of file                    " RESET "-> %ld \n", size);
	fprintf(stdout, GREEN "\tinode number of file            " RESET "-> %d \n", (int )stat_buf.st_ino);
    fprintf(stdout, GREEN "\tUser ID of owner                " RESET "-> %d \n", (int )stat_buf.st_uid);
    fprintf(stdout, GREEN "\tGroup ID of owner               " RESET "-> %d \n", (int )stat_buf.st_gid);
    fprintf(stdout, GREEN "\tBlock Size of file              " RESET "-> %d \n", (int )stat_buf.st_blksize);
    fprintf(stdout, GREEN "\tNumber of 512B blocks allocated " RESET "-> %d \n", (int )stat_buf.st_blocks);
	fprintf(stdout, "\n");
	

	// Making a VLA based on the file size
	char vla[size + 1] ;
	size_t bytes_read = 0;

	// reading contents of the file
	bytes_read = read(fd, vla, size);
	if (bytes_read != size)
	{
		fprintf(stderr, RED"[-] Error: " RESET "bytes read are less than said \n");
		exit(0x14);
	}

	// displaying contents of the file on STDOUT	
    fprintf(stdout, YELLOW "\n\n%s\n\n" RESET, vla);
	fprintf(stdout, BLUE "Number of bytes read:" RESET " %ld \n\n", bytes_read);	

	//closing the file
	close(fd);

return 0;
}
