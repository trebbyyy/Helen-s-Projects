#include "mysync.h"
#include "list.h"
#include "hashtable.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>

// Define an optlist of valid command-line switches 
#define OPTLIST "anprvi:o:" 
// Initialise all the switches to be false - not provided
bool aflag = false, nflag = false, pflag = false, rflag = false, vflag = false, iflag = false, oflag = false; 

// Initialise the number of -i and -o patterns provided to 0, and the pattern arrays to NULL - empty
char** iPatterns = NULL;
char** oPatterns = NULL;
size_t iNumPatterns = 0;
size_t oNumPatterns = 0;

// Initialise the number of directories to zero and the directory array to NULL - empty
int dirs = 0;
const char **dirArray = NULL;

/**
 * Receives and validates command-line arguments.
 * 
 * Parameters:
 *   argc - The number of command-line arguments, including the program name
 *   argv - An array of strings containing the command-line arguments
 */
int main(int argc, char *argv[]) {
    // Check if the minimum number of command-line arguments is met
    if (argc < 3) {
        printf("Usage: %s sysconfig-file command-file\n", argv[0]);
        printf("Not enough command-line args.\n");
        exit(EXIT_FAILURE);
    }
    
    // Define an array of SWITCH structures to handle command-line switches
    struct SWITCH switchesList[] = {
        {'a', &aflag},
        {'i', &iflag},
        {'n', &nflag},
        {'o', &oflag},
        {'p', &pflag},
        {'r', &rflag},
        {'v', &vflag},
    };

    int opt;
    // Iterate through the command-line arguments to find strings
    while ((opt = getopt(argc, argv, OPTLIST)) != -1) {
        bool switchFound = false;
        // Iterate through the SWITCH array to find a matching switch
        for (int i = 0; i < sizeof(switchesList) / sizeof(switchesList[0]); i++) {
            if (opt == switchesList[i].option) {
                if (switchesList[i].flag != NULL) { // If a valic switch was provided, set the corresponding flag
                    *(switchesList[i].flag) = true; 
                }
                switchFound = true;
                break;
            }
        }
        if (!switchFound) {
            printf("Unknown switch provided: %c\n", opt);
            exit(EXIT_FAILURE);
            // Handle unknown switches here
        }
        if (opt == 'i' || opt == 'o') {
            if (optarg) {
                // Save the pattern to the corresponding array
                if (opt == 'i') {
                    add_pattern(&iPatterns, &iNumPatterns, optarg);
                } else if (opt == 'o') {
                    add_pattern(&oPatterns, &oNumPatterns, optarg);
                }
            } else {
                printf("Option -%c requires an argument.\n", opt);
                exit(EXIT_FAILURE);
            }
        }
    }

    // Ensure the -v option is set in the case of the -n optino being set
    if (nflag) vflag = true;

    dirArray = NULL; // Create an array of directories
    HASHTABLE *htable = hashtable_new(); // Create a new hashtable

    // Iterate through the command-line arguments to find directories 
    for (int i = optind; i < argc; i++) {
        if (valid_directory(argv[i])) {
            read_directory(argv[i], htable); // Read the directory contents
            dirArray = add_directory(dirArray, &dirs, argv[i]); // Add the directory to the array
        } else {
            printf("Invalid directory: %s\n", argv[i]);
            // Terminates the program if an invalid directory was provided
            exit(EXIT_FAILURE);
        }
    }

    // Terminates the program if less than two directories were provided
    if (dirs < 2) {
        printf("Not enough directories specified\n");
        exit(EXIT_FAILURE);
    }

    // Execute functions based on the provided data
    execute_functions(htable);

    // Free the dynamically allocated memory for iPatterns
    for (size_t i = 0; i < iNumPatterns; i++) {
        free(iPatterns[i]);
    }
    free(iPatterns);

    // Free the dynamically allocated memory for oPatterns
    for (size_t i = 0; i < oNumPatterns; i++) {
        free(oPatterns[i]);
    }
    free(oPatterns);
    
    free(dirArray);
    free(htable);
    return 0;
}
