// Author : Abhinav Thakur
// Email  : compilepeace@gmail.com

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


char *get_timestamp()
{
	time_t current = time(NULL);
	
	return asctime(localtime (&current));
}


int main(int argc, char *argv[])
{
	int fd = 0;
	char buffer[100];
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;

	char *filename = argv[1];

	// Create file
	fd = open( filename, O_WRONLY | O_CREAT | O_APPEND , mode);

	sprintf(buffer, "Current time : %s\n", get_timestamp());
	printf("Writing %s--> %s\n", buffer, filename);
	write(fd, buffer, strlen(buffer));

	close(fd);	

return 0;	
}

