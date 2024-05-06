#include "mysync.h"
#include "list.h"
#include "hashtable.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <dirent.h>

#include <sys/param.h>
#include <sys/stat.h>
#include <utime.h>
#include "hashtable.h"

/**
 * Find the directory path with the newest modification time from a linked list
 *
 * Parameters:
 * list - The linked list of directories, each containing a modification time
 * 
 * Returns:
 * The directory path with the newest modification time, or NULL if the list is empty
 */
const char *newestModTime(LIST* list) {
    time_t *newTime = list->modification_time; // Initialize 'newTime' with the first directory's modification time
    const char *dirpath = list->dirPath; // Initialize 'dirpath' with the first directory's path

    // Check if the list is not empty.
    if (list != NULL) {
        while(list != NULL) {
            // Compare the modification time of the current directory with 'newTime.'
            if (list->modification_time > newTime) {
                // Update 'dirpath' and 'newTime' if a newer modification time is found
                dirpath = list->dirPath;
                newTime = list->modification_time;
            }
            list = list->next; // Move to the next node in the linked list
        }
    }

    return dirpath;
}

/**
 * Execute file copying based on the information stored in the hashtable
 *
 * Parameters:
 * htable - The hashtable containing information about files and directories
 */
void execute_functions(HASHTABLE *htable) {
    // Iterate through the hashtable array
    for (int i = 0; i < HASHTABLE_SIZE; i++) {
        if (htable[i] != NULL) {
            if (vflag) printf("\nConsidering filename: %s\n", htable[i]->fileName);

            // Retrieve the linked list of directories associated with the file
            LIST *list = hashtable_retrieve(htable, htable[i]->filePath);

            // Find the directory path with the newest modification time from the list
            const char *dPath = newestModTime(list);

            // Display the directory with the newest modification time.
            if (vflag) printf("Directory with the newest modification time: %s\n", dPath);

            // Iterate through the directories.
            for (int j = 0; j < dirs; j++) {
                const char *dir = dirArray[j];

                // Check if the current directory is different from the one with the newest modification time
                if (strcmp(dir, dPath) != 0) {
                    // If the -n option was not provided, copy the file from the directory with the newest modification time to the current directory
                    if (!nflag) create_file(dPath, dir, htable[i]->filePath, htable[i]->permissions, htable[i]->modification_time);
                    if (vflag) printf("Copy %s from location: %s to location: %s\n", htable[i]->fileName, dPath, dir);
                }

                
            }

            // Free the linked list
            list_free(list);
        }
    }
}
