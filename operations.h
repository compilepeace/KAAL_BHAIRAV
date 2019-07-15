#pragma once


// Put all the MACROS, function prototypes, globally used variables and constants in this header file
// Make a function/variable static so that it cannot be called/used by any function outside the scope
// of the current source file (.c)

// header files should declare not define



// Using extern keyword we cannot initialize these variables here, they are here just to link them to
// other .c files (i.e. they can be used/manipulated via other C files also). They can be initialized
// and manipulated in other .c files only, not right here.
extern long int dir_count;
extern long int file_count;


typedef struct Node
{
	char *name;			// It will store AbsPath of filename dynamically in heap segment (calloc)
	struct Node *next;	// The address of next node in linked list containing filenames

} FileList;


// Sharing *head of the FileList so that any .c file outside file_tamper.c is able to parse
// the linked ist and perform operations on file.
extern FileList *head;


// dir_tamper.c
void DirEntriesParse(char *);

// file_tamper.c
void 		FileCreateNode(char *);
void 		AddNode(FileList *, char *);
FileList 	*AllocMemory(char *, FileList *);
void		PrintLinkedList(FileList *);
const char	*GetFileType(char *);

// misc_routines.c
char *GetAbsPath(char *, char *);
