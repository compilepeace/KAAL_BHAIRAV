#pragma once


// Put all the MACROS, function prototypes, globally used variables and constants in this header file


// dir_tamper.c
void DirEntriesParse(char *);

// file_tamper.c
const char *GetFileType(char *, char *);

// misc_routines.c
char *GetAbsPath(char *, char *);
