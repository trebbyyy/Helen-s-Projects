# mysync.c

## Description
`mysync` is a command-line utility program designed to synchronize the contents of two or more directories. It compares files in specified directories and ensures that they are identical, based on their filenames and contents.

## Usage
1. make
2. ./mysync [options] directory1 directory2 [directory3 ...]

-a: Synchronize all files, including hidden and configuration files.
-i pattern: Ignore files matching the specified pattern.
-n: Identify files to be copied but do not synchronize them (setting the -n option also sets the -v option)
-o pattern: Synchronize only files matching the specified pattern.
-p: Preserve the modification time and file permissions of copied files.
-r: Recursively process directories found within the specified directories.
-v: Enable verbose output.

## File Patterns

Globbing is used to specify file patterns. Use single quotes to enclose file patterns. For example:

-o '*.[ch]': Synchronize only C source files.

Examples:

- Synchronise all files ending with .c or .h:
    ./mysync  -o  '*.[ch]'  /mnt/c/Users/Helen/project2/hello  /mnt/c/Users/Helen/project2/world

- Synchronize files between folders "hello" and "world":
    ./mysync /mnt/c/Users/Helen/project2/hello  /mnt/c/Users/Helen/project2/world

- Identify files to be synchronized without actually synchronizing them:
    ./mysync -n -r /mnt/c/Users/Helen/project2/hello   /mnt/c/Users/Helen/project2/world

- Synchronize directories recursively and preserve file attributes:
    ./mysync -p -v /Volumes/myUSB   /mnt/c/Users/Helen/project2/world


