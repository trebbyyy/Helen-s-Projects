#include "mysync.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <utime.h>
#include <errno.h>
#include <libgen.h>
#include <time.h>

void create_file(const char *sourceDir, const char *destDir, const char *sourceFile, mode_t *permissions, time_t *modificationTime);
/**
 * Recursively creates directories along the given path
 *
 * Parameters:
 * path - The path for which directories should be created
 * 
 * Return:
 * 0 if successful, -1 on error
 */
int create_directories(const char *path) {
    char dir[1024];
    strcpy(dir, path);
    
    // Remove trailing slashes
    if (dir[strlen(dir) - 1] == '/')
        dir[strlen(dir) - 1] = '\0';

    char *dir_copy = dir;
    char *parent = dirname(dir_copy);

    // Check if the parent directory is not the root directory ("/")
    if (strcmp(parent, "/") != 0) {
        // Attempt to create the parent directory with permissions 0755
        if (mkdir(parent, 0755) == -1) {
            if (errno != EEXIST) { // Check for errors other than directory already existing (EEXIST)
                perror("Error creating directory");
                exit(EXIT_FAILURE);
            }
        }

        // Recursively create parent directories.
        create_directories(parent);
    }
    return 0;
}


/**
 * Copies a file from one directory to another, creating directories on the path if required
 * 
 * Parameters:
 *   sourceDir - The directory path where the source file is located
 *   destDir - The directory path where the copied file is to be placed
 *   sourceFile - The name of the file to be copied
 *   permissions - The file permissions to be set
 *   modificationTime - The modification time to be set
 */
void create_file(const char *sourceDir, const char *destDir, const char *sourceFile, mode_t *permissions, time_t *modificationTime) {
    // Create full source and destination paths
    char sourcePath[1024];
    char destPath[1024];
    snprintf(sourcePath, sizeof(sourcePath), "%s%s", sourceDir, sourceFile);
    snprintf(destPath, sizeof(destPath), "%s%s", destDir, sourceFile);

    // Create directories in the destination path if they don't exist
    if (create_directories(destPath) == -1) {
        return;
    }

    // Open source file for reading
    FILE *source = fopen(sourcePath, "rb");
    if (source == NULL) {
        perror("Error opening source file");
        return;
    }

    // Open destination file for writing
    FILE *destination = fopen(destPath, "wb");
    if (destination == NULL) {
        perror("Error opening destination file");
        fclose(source);
        return;
    }

    // Set permissions and modification time if pflag is enabled
    if (pflag) {
        // Set file permissions
        int permStatus = chmod(destPath, *permissions); // Dereference permissions
        if (permStatus == 0 && vflag) {
            printf("Permissions set for file %s in %s\n", sourceFile, destDir);
        } else {
            perror("Error setting file permission");
        }

        // Set modification time
        struct utimbuf newTimes;
        newTimes.actime = newTimes.modtime = *modificationTime; // Dereference modificationTime
        if (utime(destPath, &newTimes) != 0) {
            perror("Failed to set modification time");
        } else {
            printf("Modification time of %s set successfully.\n", sourceFile);
        }
    }

    // Allocate memory for copying file content
    char buffer[1024]; // Initialize buffer
    size_t chunkSize = sizeof(buffer);

    while (1) {
        size_t bytesRead = fread(buffer, 1, chunkSize, source);
        if (bytesRead == 0) {
            break;  // End of file
        }

        fwrite(buffer, 1, bytesRead, destination);
    }

    // Close files
    fclose(source);
    fclose(destination);
}
