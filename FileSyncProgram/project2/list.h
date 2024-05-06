#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <sys/types.h>
#include <time.h>

//  A HELPFUL PREPROCESSOR MACRO TO CHECK IF ALLOCATIONS WERE SUCCESSFUL
#define CHECK_ALLOC(p) if(p == NULL) { perror(__func__); exit(EXIT_FAILURE); }


//  OUR SIMPLE LIST DATATYPE - A DATA ITEM, AND A POINTER TO ANOTHER LIST
typedef struct _list {   
     char           *fileName;
     char           *filePath;
     char           *dirPath;
     mode_t         *permissions;
     time_t         *modification_time;
     struct _list   *next;
} LIST;


//  THESE FUNCTIONS ARE DECLARED HERE, AND DEFINED IN list.c :

//  'CREATE' A NEW, EMPTY LIST
extern	LIST	*list_new(void);

//  ADD A NEW (STRING) ITEM TO AN EXISTING LIST
extern	LIST	*list_add(LIST *list, char *fileName, char *filePath, const char *dirPath, mode_t *permissions, time_t *modification_time);

//  DETERMINE IF A REQUIRED ITEM (A STRING) IS STORED IN A GIVEN LIST
extern	LIST	 *list_find(LIST *list, char *wanted);

//  PRINT EACH ITEM (A STRING) IN A GIVEN LIST TO stdout
extern	void	 list_print(LIST *list);

extern    void  list_free(LIST* list);

#endif // LIST_H


