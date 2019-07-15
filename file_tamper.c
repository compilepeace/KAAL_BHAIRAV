// Author : Abhinav Thakur
// Email  : compilepeace@gmail.com

// Description : This module contains all the operations that are being performed on files.
  

/*

struct stat {
               dev_t     st_dev;     // ID of device containing file 
               ino_t     st_ino;     // inode number 
               mode_t    st_mode;    // protection 
               nlink_t   st_nlink;   // number of hard links 
               uid_t     st_uid;     // user ID of owner 
               gid_t     st_gid;     // group ID of owner 
               dev_t     st_rdev;    // device ID (if special file) 
               off_t     st_size;    // total size, in bytes 
               blksize_t st_blksize; // blocksize for file system I/O 
               blkcnt_t  st_blocks;  // number of 512B blocks allocated 
               time_t    st_atime;   // time of last access 
               time_t    st_mtime;   // time of last modification 
               time_t    st_ctime;   // time of last status change 
           };
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "color.h"

#include "operations.h"



// Initially marks as empty list
FileList *head = NULL; 
FileList *current_node = NULL;



// Create a new node to store Absolute path for filename
void FileCreateNode(char *filepath)
{

	// Check for the filepath length
	if (strlen(filepath) > 499)
	{
		fprintf(stderr, RED"[-]"RESET"In %s : %s -> %s too long to be stored into FileList\n", __FILE__, __FUNCTION__, filepath);
		exit(0x51);
	}

	
	AddNode(current_node, filepath);
}



// Checks if the node is the initial or intermediate node and sets the current_node pointer accordingly.
void AddNode(FileList *current, char *filepath)
{


	// Initialize the FilesList if its currently empty 
    if (head == NULL)
    {
		head = AllocMemory(filepath, NULL);
		current_node = head;
	}

	
	// Add to the next of current node and point current_node to the new node added
	else if (head != NULL)
	{
		current_node->next = AllocMemory(filepath, NULL);
		current_node = current_node->next;
	}

}



// Creates and initializes a new node in heap segment. Returns the address of new malloc'd memory.
FileList *AllocMemory(char *filepath, FileList *next_node_address)
{

	FileList *node = (FileList *) malloc( sizeof(FileList) );
   
	if (node == NULL)
	{
    	// Malloc failed. Exiting
		fprintf(stderr, RED"[-] %s : %s while malloc(). Exiting ... \n\n", __FILE__, __FUNCTION__);
		exit(0x50);
	}

		
		// Clear out the malloced memory and store the (filepath) and (address of next node) in it
		memmove(node, "0", sizeof(FileList));
		
		node->name = (char *) calloc(strlen(filepath), 1);	
		strncpy(node->name, filepath, strlen(filepath));

		node->next = next_node_address;


	return node;
}



// Prints the linked list nodes
void PrintLinkedList(FileList *start)
{
	
	FileList *ptr = start;	
	

	fprintf(stdout, "\n\n");

	do
	{
		fflush(stdout);
		fprintf(stdout, "| "YELLOW"%s"RESET" : "RED"%p"RESET" | -> ", ptr->name, ptr->next);
		ptr = ptr->next;
	
	} while (ptr != NULL);

}



// Function returns filetype of a file reffered by its absolute pathname
const char *GetFileType(char *absolute_path)
{

        // To get proper results (i.e. filetype from getfiletype's S_ISBLK() etc.) use the absolute
        // pathname for file (i.e. from / directory) to identify correct file type, otherwise we 
        // won't get proper filetype.       


		struct stat s;


    	// extract the properties of the file into struct stat object - 's'
    	int status = lstat(absolute_path, &s);


    		// Here S_IFMT is the bitmask for filetype field. Alternatively we could use 
			// S_ISDIR(s.st_mode) to check if the current file is a directory
   			switch(s.st_mode & S_IFMT)
    		{
    		    case S_IFBLK:   return "Block device";
    		                    break;
    	    	case S_IFCHR:   return "Character device";
    	    	                break;
    	    	case S_IFDIR:   return "Directory";
    	    	                break;
    	    	case S_IFIFO:   return "FIFO/PIPE";
    	    	                break;
    	    	case S_IFLNK:   return "Symbolic link";
    	    	                break;
    	    	case S_IFREG:   return "Regular file";
    	    	                break;
    	    	case S_IFSOCK:  return "Socket";
    	    	                break;
   	    		default:        return "Unknkown??";
        		                break;
    		}
}
