#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "list.h"  

/**
* Creates a new list with null value
*/
LIST *list_new(void)
{
    return NULL;
}

/**
 * Determine if the wanted file is stored in a given linked list
 * 
 * Parameters:
 *  list - searches through linked list to see if the file exists in it
 *  wanted - the file desired
 *
 * Returns:
 *  If the requested file is found, returns the list containing its information
 *  If the requested file is not found, returns NULL;
 */
LIST *list_find(LIST *list, char *wanted)
{
    while(list != NULL) { //While node is not null, compares the file path with the wanted file path
	if(strcmp(list->filePath, wanted) == 0) { //If file path found, return the list
	    return list;
	}
	list	= list->next;
    }
    return NULL; //If file path not found, return NULL
}


/**
* Allocates space for a new list item and saves the file metadata into it
*
* Parameters: All file metadata 
*
* Returns: 
*   List with all the file information filled in
*
*/

LIST *list_new_item(char *fileName, char *filePath, const char *dirPath, mode_t *permissions, time_t *modification_time)
{
    LIST *new       = calloc(1, sizeof(LIST) ); //For each thing, allocates memory for it and checks if alloc worked
    CHECK_ALLOC(new);

    new->fileName     =  strdup(fileName); //...And sets the value for it i.e. copying the dirpath across from the parameters given in dirread 
    CHECK_ALLOC(new->fileName);

    new->next       =  NULL; //Sets the list-> next variable to NULL. 

    new->filePath    = strdup(filePath);
    CHECK_ALLOC(new->filePath);

    new->dirPath    = strdup(dirPath);
    CHECK_ALLOC(new->dirPath);

    new->permissions = malloc(sizeof(mode_t));
    CHECK_ALLOC(new->permissions);
    *(new->permissions) = *permissions;

    new->modification_time = malloc(sizeof(time_t));
    CHECK_ALLOC(new ->modification_time);
    *(new->modification_time) = *modification_time;

    return new;
}


/**
*
* Adds new list containing file and its metadata to linked list. If the list node doesn't exist, makes a new one
*
* Parameters: 
*   List - list given to it with meta data assigned from list_item_add();
*   File metadata
*
* Returns:
*   List given to it if file found in the list
*   New linked list created and node added to linked list if the file is not found
*
*/
LIST *list_add(LIST *list, char *fileName, char *filePath, const char *dirPath, mode_t *permissions, time_t *modification_time)
{
    if(list_find(list, filePath) && (list->dirPath == dirPath)) {  //Only add each item once
        return list;
    }
    else {  //Add new item to head of list
        LIST *new   = list_new_item(fileName, filePath, dirPath, permissions, modification_time);  //Just getting the list from when you allocated memory in list_new_item()
        new->next   = list; 

        return new;
    }
}

/**
*
* Prints each node in a linked list to stdout
* 
* Parameters:
*   list - the list to be printed
*
* Returns a printed list
*
*/
//  PRINT EACH ITEM (A STRING) IN A GIVEN LIST TO stdout
void list_print(LIST *list)
{
    if(list != NULL) {
        while(list != NULL) {
	    printf("File Name: %s\n", list->fileName);
        printf("File Path: %s\n", list->filePath);
        printf("File Directory Path: %s\n", list->dirPath);
        printf("Permissions: %o\n", *(list->permissions));
        printf("Modification Time: %s\n", ctime(list->modification_time)); //Converts time to human-readable form
    
	    list	= list->next; //Sets list to be printed to next node of linked list until the end is reached
        }
    }
}

/**
*
* Frees memory for each file and its metadata
*
* Parameters:
*   list - the list to be freed
*/
void list_free(LIST *list) {
    LIST *temp; //temp variable to hold the pointer to the memory address of the list
    while (list != NULL) {
        free(list->fileName);
        free(list->filePath);
        free(list->dirPath);
        free(list->permissions);
        free(list->modification_time);
        temp = list; //Saves pointer to the list itself so it can be freed after allocating the list to the next one in the linked list
        list = list->next;
        free(temp); //Frees memory allocated to the list itself
    }
}

