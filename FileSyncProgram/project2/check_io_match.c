#include "mysync.h"
#include <stdbool.h>
#include <regex.h>
#include <stdio.h>

/**
 * Convert a filename pattern (a glob) to a regular expression.
 * Filename patterns may not include directory separators.
 *
 * Parameters:
 * glob - The filename pattern (glob) to convert to a regular expression.
 *
 * Returns:
 * A regular expression dynamically allocated in memory on success, or NULL on failure.
 */
char * glob2regex(char *glob) {
    char *re = NULL;

    if(glob != NULL) {
	re	= calloc(strlen(glob)*2 + 4, sizeof(char));
        if(re == NULL) {
	    return NULL;
	}

	char *r	= re;

	*r++	= '^';
	while(*glob != '\0')
	    switch (*glob) {
		case '.' :
		case '\\':
		case '$' : *r++ = '\\'; *r++ = *glob++;	break;
		case '*' : *r++ = '.';  *r++ = *glob++;	break;
		case '?' : *r++ = '.'; glob++;		break;
		case '/' : free(re);
			   re	= NULL;
			   break;
		default  : *r++ = *glob++;
			   break;
	    }
	if(re) {
	    *r++	= '$';
	    *r		= '\0';
	}
    }
    return re;
}

/**
 * Check if a given filename matches a glob pattern using regular expressions.
 *
 * Parameters:
 * glob - The glob pattern to match against.
 * ilename - The filename to check for a match.
 * 
 * Returns:
 * True if there is a match, false otherwise.
 */
bool check_match(char *glob, const char *filename) {
	// Convert the glob pattern to a regular expression.
	char *re = glob2regex(glob);

	// Declare a regular expression structure and an array to store match position.
	regex_t regex;
	regmatch_t pmatch[1]; 
	if (re != NULL) {
		int ret = regcomp(&regex, re, 0);
		if (ret == 0) {
			//Execute the regular expression match on the filename.
			ret = regexec(&regex, filename, 1, pmatch, 0);
			if (!ret) {
				return true;
			} else if (ret == REG_NOMATCH) {
				return false;
			} else {
				// Handle errors in regex execution, print error message to stderr.
				char error_buffer[100];
				regerror(ret, &regex, error_buffer, sizeof(error_buffer));
				fprintf(stderr, "Regex execution error: %s\n", error_buffer);
				return false;
			}
		} else {
			// Handle errors in regex compilation, print error message to stderr.
			if (vflag) {
				char error_buffer[100]; // A buffer to store the error message
				regerror(ret, &regex, error_buffer, sizeof(error_buffer));
				fprintf(stderr, "Error compiling regex: %s\n", error_buffer);
			}
			return false;
		}
	} else {
		if (vflag) printf("Invalid glob: %s\n", glob);
		exit(EXIT_FAILURE);
	}

	// Free the regex structure
	regfree(&regex);
}

/**
 * Add a new pattern to an array of patterns
 *
 * Parameters:
 * patterns - A pointer to the array of patterns
 * numPatterns - A pointer to the number of patterns in the array
 * pattern - The pattern to add to the array
 */
void add_pattern(char*** patterns, size_t* numPatterns, const char* pattern) {
	//Reallocate memory for the patterns array to accommodate a new pattern.
    (*patterns) = (char**)realloc((*patterns), (*numPatterns + 1) * sizeof(char*));
	// Check if memory allocation fails, and terminate the program if so
    if ((*patterns) == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

	//Allocate memory for the new pattern string and copy the provided pattern.
    (*patterns)[*numPatterns] = (char*)malloc((strlen(pattern) + 1) * sizeof(char));
	// Check if memory allocation fails, and terminate if so
    if ((*patterns)[*numPatterns] == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

	// Copy the provided pattern to the newly allocated memory.
    strcpy((*patterns)[*numPatterns], pattern);
	// Increment the quantity of patterns in the array
    (*numPatterns)++;
}