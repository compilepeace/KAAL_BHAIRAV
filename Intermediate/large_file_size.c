// Author : Abhhinav Thakur
// Email  : compilepeace@gmail.com

// Description : This program creates a large sized file (1 MB default size) by using lseek() to 
//				 seek to an offset beyond the EOF and then writing "\x00" character at that offset.
//				 This fills in all the gap between the starting of file and that offset wit 0x00.
//				 Note : This is only working with O_CREAT | O_EXCL flags (i.e. with newly created
//				 files and NOT on existing files).


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <fcntl.h>
#include <sys/types.h>

#define SIZE 1024*1024


int main(int argc, char *argv[])
{
	size_t file_size = 0;
	off_t old_position = 0, new_position = 0x00;

	
	fprintf(stdout, "[+] Verifying command line arguments \n");

	if (argc < 2)
	{
		fprintf(stderr, "[-] Usage: %s <filename> <expand_size> (in MB) \n", argv[0]);
		exit(0x10);
	}

	// If size not provided by user, assume it to be the macro SIZE (1GB)
	if (argc != 3 )
	{
		file_size = (size_t )SIZE;
		fprintf(stdout, "[+] File will be expanded by default size : %ld Bytes\n", file_size);
	}

	// else set the file size to be user's given value
	else
	{
		file_size = (size_t )atoi(argv[2]) * 1024 * 1024;
		fprintf(stdout, "[+] File will be expanded by %ld Bytes \n", file_size);
	}


	// open the file
	fprintf(stdout, "[+] Opening file : %s \n", argv[1]);
	int fd = open(argv[1],  O_WRONLY | O_CREAT | O_EXCL, 0777);
	if (fd == -1)
	{
		fprintf(stderr, "[-] Error: while opening '%s' \n", argv[1]);
		exit(0x11);
	}

	// seek to 0 bytes from the current position
	old_position = lseek(fd, 0x00, SEEK_CUR);
	
	if (old_position == -1)
	{
		fprintf(stderr, "[-] Error: while getting current position \n");
		exit(0x12);
	}	
	
	fprintf(stdout, "[+] Current position : 0x%08lx \n", old_position);
	
	// seek to file_size bytes beyond the EOF
	new_position = lseek(fd, file_size - 1, SEEK_SET);
	
	if (new_position == -1)
	{
		fprintf(stderr, "[-] Error: lseeking to %ld bytes from the current position", file_size - 1);
		exit(0x13);
	}
	
	fprintf(stdout, "[+] Setting current position to 0x%08lx \n", new_position);

	// Write a \x00 byte on that location 
	fprintf(stdout, "[+] Writing 0x%02x at offset 0x%08lx from starting of file \n", 0x00, new_position);
	write(fd, "\x00" , 1);
	
	// close the file
	close(fd);

return 0;
}
