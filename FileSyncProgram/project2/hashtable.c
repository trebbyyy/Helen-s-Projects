#include "hashtable.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
*
* Formats complete file paths to exclude the parent directory of the directory to be synced 
*
* Parameters:
*   filePath: the complete file path 
*   og_dir_name: the directory path input into shell
*
* Returns:
*   Formatted file path
*/

char *file_path_format(char *filePath, const char *og_dir_name) {
    
    const char *lastSlash = strrchr(og_dir_name, '/'); //Stores location (pointer) of where the last forward slash is
    
    if (lastSlash != NULL) {                                    
        size_t length = strlen(lastSlash + 1);

        char dirName[length + 1];
        strcpy(dirName, lastSlash + 1);

        const char *cutFilePath = strstr(filePath, dirName); //If location of where slash is valid, store another pointer to where the directory name starts within the file path name

        if (cutFilePath != NULL) {                              
            size_t startIndex = cutFilePath - filePath + strlen(dirName);

            char *formatted_filePath = malloc(strlen(filePath) - startIndex + 1); //If location of where directory name starts is valid, 
                                                                                  //Allocate memory to store the formatted file path
            CHECK_ALLOC(formatted_filePath); //Checks memory allocation
            if (formatted_filePath != NULL) {                                 
                strcpy(formatted_filePath, filePath + startIndex);
                return formatted_filePath; //Return formatted file path                              
            } else {
                                                                                    
                printf("Memory allocation failed.\n"); //Handle memory allocation failure
                return NULL;
            }
        }
    } else {
        printf("No '/' found in og_dir_name.\n"); //Prints error if no "/" found in the file path
    }
    
    return NULL; //Directory name not found in the filepath.
}

/**
*
* Hashes input file path
*
* Parameters:
*   File path
*
* Returns:
*   Hashed file path
*/
uint32_t hash_string(char *filePath) //Hashes input filepath
{
    uint32_t hash = 0;

    while(*filePath != '\0') {
        hash = hash*33 + *filePath;
        ++filePath;
    }
    return hash;
}

/**
* Creates new hashtable and validates memory allocation
* 
* No parameters
*
* Returns:
*   Pointer to new hashtable
*/
HASHTABLE *hashtable_new(void) {
    HASHTABLE   *new = calloc(HASHTABLE_SIZE, sizeof(HASHTABLE)); //allocates memory for hashtable and sets memory size to hashtable size

    CHECK_ALLOC(new); //Checks memory allocation
    return new;
}

/**
* Adds a file and its metadata to the hashtable
* 
* Parameters:
*   Hashtable - hashtable holding all file information
*   File metadata - file name, directory path, permissions, modification time
*
* Returns:void
*/
void hashtable_add(HASHTABLE *hashtable, char *fileName, char *filePath, const char *dirPath, mode_t *permissions, time_t *modification_time)
{ 
    uint32_t h   = hash_string(filePath) % HASHTABLE_SIZE; //Computes hash number 

    *permissions = *permissions & 0777;

    //Adds metadata to node for that file in the hashtable in index corresponding to the hash number
    hashtable[h] = list_add(hashtable[h], fileName, filePath, dirPath, permissions, modification_time);

}

 /**
 *
 * Determine if a given file already exists in a given hashtable
 *
 * Parameters:
 *  Hashtable
 *  File path
 *
 * Returns:
 *  True if file path found in hashtable, false if not
 */
bool hashtable_find(HASHTABLE *hashtable, char *filePath)
{
    uint32_t h	= hash_string(filePath) % HASHTABLE_SIZE;     //Hashes file path

    return list_find(hashtable[h], filePath);
}

/**
*
* Retrieves the node containing information about a specified file
*
* Parameters:
*   Hashtable
*   File path
*
* Returns:
*   Node (list) containing information about the file 
*/
LIST* hashtable_retrieve(HASHTABLE* htable, char* filePath) {
    uint32_t h = hash_string(filePath) % HASHTABLE_SIZE;
    LIST* result = NULL;
    while (htable[h] != NULL) {
        //Check if the file path mathces the current node's file path
        if (strcmp(htable[h]->filePath, filePath) == 0) { 
            // If a match is found, add the node to the result linked list
            if (result == NULL) { //If the file path isn't found, create a new result list
                result = list_new();
            }

            //Adds result list to linked list for that file entry in the hash table
            result = list_add(result, htable[h]->fileName, htable[h]->filePath, htable[h]->dirPath, htable[h]->permissions, htable[h]->modification_time);
        }
        //Move to next node in the result list
        htable[h] = htable[h]->next;
    }
    //store the result list back in the hashtable at the same index
    htable[h] = result;
    return result;
}

/**
* Prints all the files from all directories stored in the hashtable
* 
* Parameters:
*   Hashtable
*
* Returns: 
*   Just prints what's in the hashtable
*/
void hashtable_print(HASHTABLE *htable) {
for (int i = 0; i < HASHTABLE_SIZE; i++) {
        if (htable[i] != NULL) {
            printf("Key: %d\n", i); // Print the key (index)
            list_print(htable[i]);   // Print the linked list at this key
        }
    }

}



