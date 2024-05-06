//  CITS2002 Project 2 2023
//  Student1:   23159504   Isabella Rowe
//  Student2:   23072751   Helen Yang

#ifndef MYSYNC_H
#define MYSYNC_H
#include "list.h"
#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/param.h>
#include <dirent.h>
#include <sys/stat.h> // Include sys/stat.h for stat function
#include <sys/types.h>
#include <string.h>   // Include string.h for string manipulation
#include <time.h>

// Initialise all the switches
extern bool aflag, nflag, pflag, rflag, vflag, iflag, oflag; 

// Initialise the -i and -o pattern arrays
extern char** iPatterns;
extern char** oPatterns;
extern size_t iNumPatterns;
extern size_t oNumPatterns;

// Initialise the command-line directory array
extern int dirs;
extern const char **dirArray;

// Initialise the structure to set the switches
struct SWITCH {
    char option;
    bool *flag;
};

// Check if a directory can be accessed
extern bool         valid_directory(const char *dirname);

// Receives and validates command-line arguments.
extern void         execute_functions(HASHTABLE *htable);

// Copies a file from one directory to another, creating directories on the path if required
extern void         create_file(const char *sourceDir, const char *destinationDir, const char *sourceFile, mode_t *permissions, time_t *modification_time);

// Check if a given filename matches a glob pattern using regular expressions.
extern bool         check_match(char *glob, const char *filename);

// Add a new pattern to an array of patterns.
extern void         add_pattern(char*** patterns, size_t* numPatterns, const char* pattern);

// Find the directory path with the newest modification time from a linked list
extern const char * newestModTime(LIST* list);

// Adds a directory to the array of directories 
extern const char **add_directory(const char **array, int *size, const char *dir);

#endif