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



// A wrapper function that sets up absolute pathname for directory entry's and calls getfiletype() on
// that directory entry
const char *GetFileType(char *user_provided_location, char *dir_entry)
{

        // To get proper results (i.e. filetype from getfiletype's S_ISBLK() etc.) use the absolute
        // pathname for file (i.e. from / directory) to identify correct file type, otherwise we 
        // won't get proper filetype.       
        char absolute_path[300];
        int location_len = strlen(user_provided_location);

        // Zero out absolute path and store user provided location
        memmove(absolute_path, "0", sizeof(absolute_path));
        strncpy(absolute_path, user_provided_location, location_len);

        // Append a '/' at the end of location provided by user
        if (absolute_path[location_len - 1] != '/')
            strncat(absolute_path, "/", 1);

        // Append directory entry
        strncat(absolute_path, dir_entry, strlen(dir_entry));



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
