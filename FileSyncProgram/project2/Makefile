PROJECT = mysync
HEADERS = mysync.h list.h hashtable.h
OBJ = mysync.o execution.o create_file.o check_io_match.o dirread.o list.o hashtable.o 

CFLAGS = -Wall -Werror
CC = cc

$(PROJECT) : $(OBJ)
	$(CC) $(CFLAGS) -o $(PROJECT) $(OBJ)

%.o : %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(PROJECT) $(OBJ)