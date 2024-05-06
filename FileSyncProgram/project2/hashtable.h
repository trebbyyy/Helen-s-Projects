#ifndef HASHTABLE_H 
#define HASHTABLE_H
#include "list.h"
#include <stdbool.h>

//Defines the hashtable size (a prime number)
#define	HASHTABLE_SIZE		997

//Defining a hashtable as a dynamically-allocated array of lists  
typedef	LIST * HASHTABLE;

//FUNCTIONS:

//Creating a hashtable and sets value to void
extern	HASHTABLE       *hashtable_new(void);

//Adding a file entry to the hashtable
extern	void            hashtable_add( HASHTABLE *hashtable, char *fileName, char *filePath, const char *dirPath, mode_t *permissions, time_t *modification_time);

//Determining if a file path exists in a hash table
extern	bool            hashtable_find(HASHTABLE *, char *filePath);

//Retrieve inidividal file from one directory's information (i.e. info for dir1/hello.txt)
extern  LIST            *hashtable_retrieve(HASHTABLE *htable, char *filePath); 

//Prints the hashtable
extern  void            hashtable_print(HASHTABLE *hashtable);

//Formats the file path to only contain information 
extern  char            *file_path_format(char *filePath, const char *og_dir_name);

//Reads the directory and stores file information in the hashtable
extern  void            read_directory(const char* dir_name, HASHTABLE *htable);

#endif