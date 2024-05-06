#include "mysync.h"
#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <sys/param.h>

/**
 * Adds a directory to the array of directories 
 * 
 * Parameters:
 * array - The directory array to update
 * size - The previous size of the array
 * dir - The directory to add to the array
 * 
 * Returns the updated array
*/
const char **add_directory(const char **array, int *size, const char *dir) {
    // Calculate the new size
    int newSize = *size + 1; // Increase size for the new directory

    // Resize the array using realloc
    const char **newArray = (const char **)realloc(array, newSize * sizeof(const char*));

    if (newArray) {
        // Set the new directory in the array
        newArray[*size] = dir;
        *size = newSize;
    }

    return newArray;
}

/**
 * Checks if a directory exists at the given path.
 *
 * Parameters:
 *   dirPath - The path to the directory to check for existence.
 * 
 * Returns:
 *   true if the directory exists, false otherwise.
 */
bool valid_directory(const char *dirPath) { 
    DIR *dirp;

    dirp = opendir(dirPath);  // Attempt to open the directory at dirPath
    if (dirp == NULL) {
        perror(dirPath);  // Print an error message if the directory does not exist
        return false;    
    } else {
        closedir(dirp);
        return true;
    }
    return false;
}

/**
 * Reads and stores the information from a directory 
 * 
 * Parameters:
 * CLdirPath - The original directory name (passed from the command line)
 * dirPath - The name of the directory to read
 * htable - The hashtable to store the information to 
 * 
*/
void process_file(const char* CLdirPath, const char* dirPath, HASHTABLE* htable, const char* filename) {
    // Declare a structure to hold file information
    struct stat file_stat;
    // Create a buffer to store the complete file path
    char filepath[MAXPATHLEN];
    // Make the complete file path by concatenating directory path and filename
    snprintf(filepath, sizeof(filepath), "%s/%s", dirPath, filename);

    // Use the stat function to get information about the file
    if (stat(filepath, &file_stat) == -1) {
        perror("Stat");
        exit(EXIT_FAILURE);
    }

    // Generate the file path relative to the base directory
    char* file_path = file_path_format(filepath, CLdirPath);
    // Add the information about the file to the hashtable
    hashtable_add(htable, (char*)filename, (char*)file_path, CLdirPath, &file_stat.st_mode, &file_stat.st_mtime);

    // Verify that the file can be found in the hashtable
    if (!(hashtable_find(htable, file_path))) {
        printf("Error: %s was not found in the hashtable.\n", filename);
    }
    free(file_path);
}

void read_directory_nested(const char* CLdirPath, const char* dirPath, HASHTABLE* htable) {
    DIR* dir;
    struct dirent* entry;

    // Attempt to open the directory for reading
    dir = opendir(dirPath);
    if (!dir) {
        perror(dirPath);
        exit(EXIT_FAILURE); // If the directory cannot be opened, terminate the program
    }

    // Iterate through the directory entries
    while ((entry = readdir(dir)) != NULL) {
        const char* filename = entry->d_name;

        // Check if the entry is a regular file, not one of the "." or ".." files, and not a hidden file, unless the -a option was given
        if (entry->d_type == DT_REG && ((aflag || filename[0] != '.') && strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0)) {
            if (oflag && iflag) {
                bool only = false;
                bool ignore = false;
                // Checks if the file matches a pattern specified after the -o option
                for (int o = 0; o < oNumPatterns; o++) {
                    if (check_match(oPatterns[o], filename)) only = true;
                }
                // Checks if the file matches a pattern specified after the -i option
                for (int i = 0; i < iNumPatterns; i++) {
                    if (check_match(iPatterns[i], filename)) ignore = true;
                }
                // Only allows files to be processed that are included in one of the -o patterns, and in none of the -i patterns
                if (only && !ignore) {
                    process_file(CLdirPath, dirPath, htable, filename);
                }
            } else if (oflag && !iflag) {
                // Checks if the file matches a pattern specified after the -o option
                for (int i = 0; i < oNumPatterns; i++) {
                    if (check_match(oPatterns[i], filename)) {
                        process_file(CLdirPath, dirPath, htable, filename);
                    }
                }
            } else if (!oflag && iflag) {
                // Checks if the file matches a pattern specified after the -i option
                for (int i = 0; i < iNumPatterns; i++) {
                    if (!check_match(iPatterns[i], filename)) {
                        process_file(CLdirPath, dirPath, htable, filename);
                    }
                }
            } else {
                // If the -i and -o options were not specified, pass all regular files to the process_file function
                process_file(CLdirPath, dirPath, htable, filename);
            }
        }

        // Check if the entry is a directory
        if (entry->d_type == DT_DIR) {
            const char* dname = entry->d_name;
            // Check that the directory is not "." or "..", and that the -r option was set, allowing recursion
            if (rflag && strcmp(dname, ".") != 0 && strcmp(dname, "..") != 0) {
                char filepath[MAXPATHLEN];
                // Make the complete directory path by concatenating the original directory and the specified directory name
                snprintf(filepath, sizeof(filepath), "%s/%s", dirPath, dname);
                // recursively call the read_directory_nested function
                if (valid_directory(filepath)) read_directory_nested(CLdirPath, filepath, htable);
            }
        }
    }

    closedir(dir);
}

/**
 * Ensures the command-line directory path can be called as the directory information is saved recursively
 * 
 * Parameters:
 * dirPath - the name of a command-line directory 
 * htable - the hashtable to store the information to 
 * 
*/
void read_directory(const char* dirPath, HASHTABLE *htable) {
    read_directory_nested(dirPath, dirPath, htable);
}