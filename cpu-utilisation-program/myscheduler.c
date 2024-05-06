#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>


//  CITS2002 Project 1 2023
//  Student1:   23072751   Helen Yang
//  Student2:   23159504   Isabella Rowe

//  myscheduler (v1.0)
//  Compile with:  cc -std=c11 -Wall -Werror -o myscheduler myscheduler.c


//  THESE CONSTANTS DEFINE THE MAXIMUM SIZE OF sysconfig AND command FILES
#define MAX_DEVICES                     4 
#define MAX_DEVICE_NAME                 20
#define MAX_COMMANDS                    10
#define MAX_COMMAND_NAME                20
#define MAX_SYSCALLS_PER_PROCESS        40
#define MAX_RUNNING_PROCESSES           50
#define MAX_LINE_SIZE                   100   

//  DEVICE DATA-TRANSFER-RATES ARE MEASURED IN BYTES/SECOND,
//  ALL TIMES ARE MEASURED IN MICROSECONDS (usecs),
//   THE TOTAL-PROCESS-COMPLETION-TIME WILL NOT EXCEED 2000 SECONDS

#define DEFAULT_TIME_QUANTUM            100

// PROCESS STATE TRANSITION TIMES
#define TIME_CONTEXT_SWITCH             5
#define TIME_CORE_STATE_TRANSITIONS     10
#define TIME_ACQUIRE_BUS                20

#define READY_TO_RUNNING                5
#define RUNNING_TO_SLEEPING             10
#define SLEEPING_TO_READY               10
#define RUNNING_TO_READY                10
#define WAITING_TO_READY                10


//  ----------------------------------------------------------------------

#define CHAR_COMMENT                    "#"

#define MYSIZE 1000

// BLOCKED QUEUE TYPES
#define READYNUM  0
#define WAITNUM   1
#define SLEEPNUM  2
#define IONUM     3

// INPUT/OUTPUT STATES
#define READY_FOR_DATABUS  0
#define ACCESSING_DATABUS  1

// READ/WRITE STATES
#define READCOM   0
#define WRITECOM  1

//INITIALISING VARIABLE NAMES TO SAVE PROGRAM AND SYSTEM CALL INFORMATION
int timeQuantum;
int cpuTime;
int sleepTime;

char line[MAX_LINE_SIZE];
char commandName[MAX_COMMAND_NAME]; 

//INITIALISING STRUCT FOR EACH DEVICE
struct DeviceStruct { 
    char name[MAX_DEVICE_NAME];
    int readSpeed;
    int writeSpeed;
}; 

//INITIALISING STRUCT FOR EACH SYSTEM CALL
struct SyslistStruct {
    //CPU TIME OF SYSCALL     
    int when;              
    //SLEEP TIME OF SYSCALL                           
    int sleepLen;                           
    //DATA SIZE OF READ/WRITE IN BYTES
    int size;
    //TIME THAT IT TAKES FOR INDIVIDUAL SYSCALL ON CPU
    int commandTime;

    // ARRAY FOR THE NAMES OF THE SYSCALL
    char syscallName[MAX_COMMAND_NAME]; 
    //ARRAY FOR THE DEVICE INFORMATION
    char device[MAX_DEVICE_NAME];       
    //ARRAY FOR THE SPAWN
    char spawnCom[MAX_COMMAND_NAME]; 
};

//INITIALISING STRUCT FOR EACH COMMAND
struct CommandStruct { 
    char name[MAX_COMMAND_NAME]; 
    //INITIALISING STRUCT WITHIN COMMANDSTRUCT TO STORE EACH SYSCALL AND ITS DETAILS
    struct SyslistStruct syslist[MAX_SYSCALLS_PER_PROCESS];         
    int numSyscalls;
};

// INITIALISING THE READY/BLOCKED QUEUES STRUCT
struct Queue {
    // ARRAY OF PROCESS IDS  
    int pidArray[MYSIZE];     
    // ARRAY OF SYSCALL IDS
    int sysArray[MYSIZE];     
    // ARRAY OF COMMAND TIMES FOR INDIVIDUAL SYSCALLS        
    int editableCT[MYSIZE];       
    // ARRAY OF PARENT IDS      
    int parent[MYSIZE];

    // ARRAYS OF TIMES FOR DIFFERENT BLOCKED STATES
    int waitingTime[MYSIZE];
    int sleepingTime[MYSIZE];
    int ioTime[MYSIZE];
    
    // ARRAY FOR READY TIMES
    int readyTime[MYSIZE];
    // ARRAY FOR READ/WRITE STATES
    int ioState[MYSIZE];

    // QUEUE CHARACTERISTICS
    int front, rear, size;
};

// INITIALISING STRUCTS FOR DEVICES FROM SYSCONFIG AND COMMANDS FROM COMMAND_FILE
struct DeviceStruct devices[MAX_DEVICES]; 

struct CommandStruct commands[MAX_COMMANDS];

//  ----------------------------------------------------------------------

// FUNCTION TO READ THE SYSCONFIG.TXT FILE AND STORE THE VALUES INTO STRUCTS
void read_sysconfig(char argv0[], char filename[])
{
    FILE *fd;

    // OPENS THE SYSCONFIG.TXT FILE
    fd = fopen(filename, "r"); 

    if (fd == NULL) {
        perror("Error opening the file");                     
    }

    int count = 0;
    while (fgets (line, sizeof(line), fd) != NULL) {
        //SAVES VARIABLES FOR EACH DEVICE TO DEVICE NAME STRUCT
        if (strncmp(line, "device", 6) == 0) {
            sscanf(line, "%*s %s %iBps %iBps", devices[count].name, &devices[count].readSpeed, &devices[count].writeSpeed);
            count++;
        }
        //SAVES TIME QUANTUM TO VARIABLE
        else if (strncmp(line, "timequantum", 11) == 0) {
            sscanf(line,"%*s %i", &timeQuantum);
        }
    }

    // CLOSES THE SYSCONFIG.TXT FILE
    fclose(fd);

}

// FUNCTION TO READ THE COMMANDS.TXT FILE AND STORE THE VALUES INTO STRUCTS
void read_commands(char argv0[], char filename[])   {

    char line[100];

    FILE *fd;

    // OPENS THE COMMANDS.TXT FILE
    fd = fopen(filename, "r");

    if (fd == NULL) {
            perror("Error opening the file");          
    }

    //INITIALISES VARIABLE TO INCREMENT THE INDEX FOR COMMAND STRUCT
    int count = 0;   
    //INITIALISES VARIABLE TO INCREMENT THE INDEX FOR SYSCALL STRUCT
    int count2 = 0; 
    //SAVES COMMAND NAME TO COMMAND IN ARRAY 
    while (fgets (line, sizeof(line), fd) != NULL) {
        if (strncmp(line, CHAR_COMMENT, 1) != 0 && !isdigit(line[1]) ) {
            sscanf(line, "%s", commandName);
            strcpy(commands[count].name, commandName);
            count2 = 0;

        }

        else if (isdigit(line[1])) {     

            //SAVES SYSCALLS AND THEIR VARIABLES TO EACH COMMAND 
            sscanf(line, "%*s %s", commandName);  
            if (strcmp(commandName,"write") == 0 || strcmp(commandName,"read") ==0 ) { 
                sscanf(line, "%iusecs %s %s %i", &commands[count].syslist[count2].when,
                commands[count].syslist[count2].syscallName, 
                commands[count].syslist[count2].device, &commands[count].syslist[count2].size); 
                if (count2 == 0) {
                    commands[count].syslist[count2].commandTime = commands[count].syslist[count2].when;
                
                } else {
                    commands[count].syslist[count2].commandTime = commands[count].syslist[count2].when - commands[count].syslist[count2 - 1].when;
                }
                
                count2++;    
            }

            else if (strcmp(commandName,"exit") == 0) {
                sscanf(line, "%iusecs %s", &commands[count].syslist[count2].when,
                commands[count].syslist[count2].syscallName);
                if (count2 == 0) {
                    commands[count].syslist[count2].commandTime = commands[count].syslist[count2].when;
                } else {
                    commands[count].syslist[count2].commandTime = commands[count].syslist[count2].when - commands[count].syslist[count2 - 1].when;
                }
                count++;
                count2++;
                commands[count-1].numSyscalls = count2;
                
                }
            else if (strcmp(commandName, "wait") == 0 ) {
                sscanf(line, "%iusecs %s", &commands[count].syslist[count2].when,
                commands[count].syslist[count2].syscallName);
                if (count2 == 0) {
                    commands[count].syslist[count2].commandTime = commands[count].syslist[count2].when;
                } else {
                    commands[count].syslist[count2].commandTime = commands[count].syslist[count2].when - commands[count].syslist[count2 - 1].when;
                }
                count2++;
                }
            
            else if (strcmp(commandName, "spawn") == 0) {
                sscanf(line, "%iusecs %s %s", &commands[count].syslist[count2].when, commands[count].syslist[count2].syscallName, commands[count].syslist[count2].spawnCom);
                if (count2 == 0) {
                        commands[count].syslist[count2].commandTime = commands[count].syslist[count2].when;
                }  else {
                    commands[count].syslist[count2].commandTime = commands[count].syslist[count2].when - commands[count].syslist[count2 - 1].when;
                }
                count2++;
            }
            else if (strcmp(commandName, "sleep") == 0) {
                sscanf(line, "%iusecs %s %iusecs", &commands[count].syslist[count2].when, commands[count].syslist[count2].syscallName, &commands[count].syslist[count2].sleepLen);
                if (count2 == 0) {
                    commands[count].syslist[count2].commandTime = commands[count].syslist[count2].when;
                } else {
                    commands[count].syslist[count2].commandTime = commands[count].syslist[count2].when - commands[count].syslist[count2 - 1].when;
                }
                count2++; 
            }

        }

    }    
    
    // CLOSES THE COMMANDS.TXT FILE
    fclose(fd);

}

//  ----------------------------------------------------------------------

// FUNCTION TO INITIALISE THE CONTENTS OF THE BLOCKED/READY QUEUE STRUCT 
void initialiseQueue(struct Queue* queue) {
    //INITIALISE ALL ELEMENTS OF ALL ARRAYS IN THE STRUCT TO BE -1
    for (int i = 0; i <= MYSIZE; i++) {

        queue->sysArray[i] = -1;
        queue->pidArray[i] = -1;
        queue->editableCT[i] = -1;
        queue->waitingTime[i] = -1;
        queue->sleepingTime[i] = -1;
        queue->readyTime[i] = -1;
        queue->ioTime[i] = -1;
        queue->ioState[i] = -1;
        queue->parent[i] = -1;

    }
    // INITIALISE FRONT TO -1 TO INDICATE AN EMPTY QUEUE 
    queue->front = -1; 
    // INITIALISE REAR TO -1 TO INDICATE AN EMPTY QUEUE 
    queue->rear = -1;  
    // INITIALISE SIZE OF THE QUEUE TO 0
    queue->size = 0;   

}

//FUNCTION TO ADD INFORMATION ABOUT A SYSCALL TO THE BLOCKED/READY QUEUES
void enqueue(struct Queue* queue, int pid, int sys, int time, int type, int comTime, int parentid, int state) {
    if (queue->size >= MYSIZE) {
        printf("Queue Struct Overloaded"); // deal with this error ?? 
        return;
    }

    // INCREMENT THE REAR POINTER 
    queue->rear = (queue->rear + 1) % MYSIZE;
    // ADD THE PROCESS ID TO THE REAR POSITION IN THE PIDARRAY
    queue->pidArray[queue->rear] = pid;
    // ADD THE SYSTEM CALL ID TO THE REAR POSITION IN THE SYSARRAY
    queue->sysArray[queue->rear] = sys;
    // ADD THE COMMAND TIME TO THE REAR POSITION IN THE EDITABLECT ARRAY
    queue->editableCT[queue->rear] = comTime;
    // ADD THE PARENT ID TO THE REAR POSITION IN THE PARENT ARRAY
    queue->parent[queue->rear] = parentid;
    // ADD THE INPUT/OUTPUE STATE TO THE REAR POSITION IN THE IOSTATE ARRAY
    queue->ioState[queue->rear] = state;
    
    // ADD THE TIME INFORMATION TO THE CORRECT QUEUE
    if (type == WAITNUM) {
        queue->waitingTime[queue->rear] = time;
    } else if (type == SLEEPNUM) {
        queue->sleepingTime[queue->rear] = time;
    } else if (type == READYNUM) {
        queue->readyTime[queue->rear] = time;
    } else if (type == IONUM) {
        queue->ioTime[queue->rear] = time;
    }
    
    //INCREMENT THE SIZE
    queue->size++;
    
    //IF IT WAS THE FIRST ELEMENT ADDED, UPDATED THE FRONT POINTER 
    if (queue->size == 1) {
        queue->front = queue->rear;
    }
}

//REMOVES THE DESIRED PROCESS FROM THE QUEUE STRUCT ARRAYS AND RETURNS THE TIMER 
int dequeue(struct Queue* queue, int * pid, int * sys, int timer, int * comTime, int * parentid, int * state) {
    if (queue->size <= 0) {
        printf("Error: Empty Queue struct\n");
        return -1; 
    }  

    // DECREMENT THE TIMER TO ENSURE ACCURATE TIME AFTER THE WHILE LOOP
    timer--;
    // INITIALISE VARIABLES TO SORT INFORMATION INTO
    int sleep = 0, wait = 0, ready = 0, io = 0, index = -1, children; 
    
    // INCREMENTS VARIABLES BASED ON WHICH TIME QUEUES HAVE VALUES IN THEM
    while (sleep == 0 && wait == 0 && ready == 0 && io == 0) {
        timer++;
        for (int i = 0; i < MYSIZE; i++) {
            // INCREMENTS SLEEP IF THERE ARE ANY COMPLETED SLEEP SYSCALLS
            if (queue->sleepingTime[i] != -1 && queue->sleepingTime[i] <= timer) {
                sleep++;
            } // INCREMENTS WAIT IF THERE ARE ANY COMPLETED WAIT SYSCALLS (HAVING NO MORE CHILD PROCESSES)
            else if (queue->waitingTime[i] != -1 && queue->waitingTime[i] <= timer) {
                children = 0;
                for (int o = 0; o < MYSIZE; o++) {
                    if (queue->parent[o] == queue->pidArray[i]) {
                        children++;
                    }
                }
                if (children == 0) {
                    wait++;
                }
            } // INCREMENTS READY IF THERE ARE ANY PROCESSES IN THE READY QUEUE
            else if (queue->readyTime[i] != -1 && queue->readyTime[i] <= timer) {
                ready++;
            } // INCREMENTS IO IF THERE ARE ANY COMPLETED INPUT/OUTPUT DATA TRANSFERS
            else if (queue->ioTime[i] != -1 && queue->ioState[i] == ACCESSING_DATABUS && queue->ioTime[i] <= timer) {
                io++;
            } 
        }
    }

    // IF THERE WERE ANY UNBLOCKED SLEEP SYSCALLS, THEY TAKE FIRST PRIORITY 
    if (sleep != 0) { 
        for (int n = 0; n < MYSIZE; n++) {
            // FINDS THE INDEX OF THE SLEEP
            if (queue->sleepingTime[n] != -1) {
                index = n;
            } 
        }
        // REMOVES THE VALUE AT THIS INDEX FROM THE SLEEPINGTIME ARRAY
        queue->sleepingTime[index] = -1;
    } // IF THERE WERE ANY UNBLOCKED WAIT SYSCALLS, THEY TAKE SECOND PRIORITY
    else if (wait != 0 && index == -1) { 
        for (int m = 0; m < MYSIZE; m++) {
            // FINDS THE INDEX OF THE WAIT
            if (queue->waitingTime[m] != -1) {
                index = m;
                // REMOVES THE VALUE AT THIS INDEX FROM THE WAITINGTIME ARRAY
                queue->waitingTime[index] = -1;
                break;
            }
        }
    } // IF THERE WERE ANY COMPLETED INPUT/OUTPUT TRANSFERS, THEY TAKE THIRD PRIORTY 
    if (io != 0 && index == -1) {
        for (int p = 0; p < MYSIZE; p++) {
            // FINDS THE INDEX OF THE I/O 
            if (queue->ioState[p] == ACCESSING_DATABUS) {
                index = p;
                // REMOVES THE VALUES AT THIS INDEX FROM THE IO ARRAYS
                queue->ioState[p] = -1;
                queue->ioTime[index] = -1;
            }
        }
        // FINDS OUT HOW MANY SYSCALLS, IF ANY, ARE WAITING FOR THE DATABUS
        int waiters = 0;
        for (int t = 0; t < MYSIZE; t++) {//
            if (queue->ioState[t] != -1) {
                waiters++;
            }
        }
        // IF THERE WERE SYSCALLS WAITING FOR THE DATABUS, THIS CODE RUNS
        if (waiters != 0) { 
            // FINDS THE SYSCALL WITH THE LOWEST READ SPEED THAT HAS BEEN WAITING THE LONGEST
            int smallest = -1, index2; 
            for (int k = 0; k < MYSIZE; k++) {
                if (queue->ioState[k] != -1 && queue->ioState[k] != ACCESSING_DATABUS) {
                    if (smallest == -1) {
                        smallest = queue->ioState[k];
                        index2 = k;
                    } else if (queue->ioState[k] < smallest) {
                        smallest = queue->ioState[k];
                        index2 = k;
                    }
                }
            }

            double rWSize, rSpeed, wSpeed; 
            // SAVES INFORMATION ABOUT THE NEXT SYSCALL TO ACCESS THE DATABUS, FOUND ABOVE
            int iopid = queue->pidArray[index2];
            int iosys = queue->sysArray[index2];
            rWSize = (double) commands[iopid].syslist[iosys].size; 

            // FINDS OUT IF THE NEXT SYSCALL TO ACCESS THE DATABUS IS A READ OR WRITE
            int readWrite;
            if (strcmp(commands[iopid].syslist[iosys].syscallName, "read")) {
                readWrite = READCOM;
            } else if (strcmp(commands[iopid].syslist[iosys].syscallName, "write")) {
                readWrite = WRITECOM;
            }
            // FINDS THE READ AND WRITE SPEEDS FOR THE NEW SYSCALL
            for (int f = 0; f <= MAX_DEVICES; f++) {
                if (strcmp(commands[iopid].syslist[iosys].device, devices[f].name) == 0) {
                    wSpeed = devices[f].writeSpeed;
                    rSpeed = devices[f].readSpeed; 
                }
            }
            // CALCULATES THE TIME THAT IT WILL TAKE FOR THIS TRANSFER TO OCCUR, DEPENDING ON THE TYPE OF SYSCALL
            double rWTime;
            if (readWrite == READCOM) {
               rWTime = rWSize / rSpeed * 1000000.00;
            } else if (readWrite == WRITECOM) {
                rWTime = rWSize / wSpeed * 1000000.00;
            }
            
            // CALCULATES THE TIME WHEN THE SYSCALL WILL BECOME UNBLOCKED AFTER THE DATA TRANSFER
            int time = timer + TIME_ACQUIRE_BUS + (int) rWTime + SLEEPING_TO_READY + 1;

            // ENQUEUES THE NEW DATA TRANSFER PROCESS 
            enqueue(queue, iopid, iosys, time, IONUM, -1, queue->parent[index2], ACCESSING_DATABUS);

            // REMOVES THE VALUES AT THIS INDEX FROM THE IO ARRAYS
            queue->ioState[index2] = -1;
            queue->ioTime[index2] = -1;
        }
    } // IF NOTHING ELSE HAS ALREADY RUN, NOW SYSCALLS IN THE READY QUEUE HAVE THE AVAILABILITY TO RUN (LOWEST PRIORITY)
    if (index == -1) {
        // FINDS THE INDEX OF THE READY SYSCALL
        for (int l = 0; l < MYSIZE; l++) {
            if (queue->readyTime[l] != -1) {
                index = l;
                break;
            }
        }
        // REMOVES THE VALUE AT THIS INDEX FROM THE READYTIME ARRAY
        queue->readyTime[index] = -1;
    }

    // UPDATES THE POINTER VARIABLES TO THE VALUES AT THE NEW INDEX, AS FOUND ABOVE
    *pid = queue->pidArray[index];
    *sys = queue->sysArray[index];
    *comTime = queue->editableCT[index];
    *parentid = queue->parent[index];
    *state = queue->ioState[index];
    
    // REMOVE THE VALUE AT THIS INDEX FROM THE PARENT ARRAY
    queue->parent[index] = -1;

    //INCREMENT THE FRONT POINTER  ]
    queue->front = (queue->front + 1) % MAX_RUNNING_PROCESSES;
    // DDECREMENT THE SIZE
    queue->size--;
    return timer;

}

// FUNCTION TO ROUND UP TO THE NEXT WHOLE DIGIT
int roundUp(double num) {
    if (num - (int) num == 0) {
        num = (int) num;
    } else {
        num = (int) num + 1;
    }
    return num;
} 

// FUNCTION THAT RUNS WHEN THE SLEEP COMMAND IS CALLED, RETURNING THE TIMER
int sleepFunction(int timer, struct Queue* readyQ, int sys, int pid, int comTime, int parentid, int state) { 
    // IF THE TIME QUANTUM WILL EXPIRE BEFORE THE SYSCALL CAN BEGIN
    if (comTime >= timeQuantum) { 
        timer += timeQuantum;
        timer += READY_TO_RUNNING; 
        timer += RUNNING_TO_READY; 
        // ADDS THE SAME SYSCALL TO THE END OF THE READY/BLOCKED QUEUE STRUCT
        enqueue(readyQ, pid, sys, timer, READYNUM, comTime - timeQuantum, parentid, state); 
    } // RUNS THE SECOND TIME THIS FUNCTION IS ACCESSED, IGNORING ACCESSES DUE TO NEW TIME QUANTUMS
    else if (comTime == -1) {
        timer += SLEEPING_TO_READY;
        // ADDS THE NEXT SYSCALL IN THE SAME PROCESS TO THE END OF THE READY/BLOCKED QUEUE
        enqueue(readyQ, pid, sys + 1, timer, READYNUM, commands[pid].syslist[sys+1].commandTime, parentid, state);
    } // RUNS THE FIRST TIME THIS FUNCTION IS ACCESSES, IGNORING ACCESSES DUE TO NEW TIME QUANTUMS
    else {
        timer += READY_TO_RUNNING; 
        timer += comTime;
        // ADDED TIME FOR THE SYSTEM TO DECIDE THAT A SYSCALL NEEDS TO BE BLOCKED
        timer += 1; 
        if (commands[pid].syslist[sys].sleepLen >= 10) {
            timer += RUNNING_TO_SLEEPING; 
            int time = timer + commands[pid].syslist[sys].sleepLen - 10;
            // ADDS THE SAME SYSCALL TO THE END OF THE READY/BLOCKED QUEUE, TO BE UNBLOCKED ONCE THE DURATION OF THE SLEEP HAS PASSED
            enqueue(readyQ, pid, sys, time, SLEEPNUM, -1, parentid, state);
        } else {
            timer += RUNNING_TO_SLEEPING; 
            timer += SLEEPING_TO_READY; 
            // ADDS THE NEXT SYSCALL IN THE SAME PROCESS TO THE END OF THE READY/BLOCKED QUEUE
            enqueue(readyQ, pid, sys + 1, timer, READYNUM, commands[pid].syslist[sys+1].commandTime, parentid, state);
        }
    }
    return timer;
}

// FUNCTION THAT RUNS WHEN THE EXIT COMMAND IS CALLED, RETURNING THE TIMER
int exitFunction(int timer, struct Queue* readyQ, int sys, int pid, int comTime, int parentid, int state) { 
    // IF THE TIME QUANTUM WILL EXPIRE BEFORE THE SYSCALL CAN BEGIN
    if (comTime >= timeQuantum) {
        timer += timeQuantum;
        timer += READY_TO_RUNNING; 
        timer += RUNNING_TO_READY; 
        // ADDS THE SAME SYSCALL TO THE END OF THE READY/BLOCKED QUEUE STRUCT
        enqueue(readyQ, pid, sys, timer, READYNUM, comTime - timeQuantum, parentid, state);
    } else {
        timer += comTime;
        timer += READY_TO_RUNNING; 
        // ADDED TIME FOR THE SYSTEM TO DECIDE THAT A SYSCALL NEEDS TO BE BLOCKED
        timer += 1; 
    }
    return timer;
}

// FUNCTION THAT RUNS WHEN THE READ OR WRITE COMMANDS ARE CALLED, RETURNING THE TIMER
int readWriteFunction(int timer, struct Queue* readyQ, int sys, int pid, int comTime, int parentid, int state, int readWrite) {
    // IF THE TIME QUANTUM WILL EXPIRE BEFORE THE SYSCALL CAN BEGIN
    if (comTime >= timeQuantum) {
        timer += timeQuantum;
        timer += READY_TO_RUNNING; 
        timer += RUNNING_TO_READY; 
        // ADDS THE SAME SYSCALL TO THE END OF THE READY/BLOCKED QUEUE STRUCT
        enqueue(readyQ, pid, sys, timer, READYNUM, comTime - timeQuantum, parentid, state);
    } // RUNS THE SECOND TIME THIS FUNCTION IS ACCESSED, IGNORING ACCESSES DUE TO NEW TIME QUANTUMS
    else if (comTime == -1) {
        timer += SLEEPING_TO_READY;
        // ADDS THE NEXT SYSCALL IN THE SAME PROCESS TO THE END OF THE READY/BLOCKED QUEUE
        enqueue(readyQ, pid, sys + 1, timer, READYNUM, commands[pid].syslist[sys + 1].commandTime, parentid, -1);
    } // RUNS THE FIRST TIME THIS FUNCTION IS ACCESSES, IGNORING ACCESSES DUE TO NEW TIME QUANTUMS
    else {
        timer += comTime;
        timer += READY_TO_RUNNING; 
        // ADDED TIME FOR THE SYSTEM TO DECIDE THAT A SYSCALL NEEDS TO BE BLOCKED
        timer += 1; 
        timer += RUNNING_TO_SLEEPING; 
        comTime = -1;
        int count = 0;
        for (int i = 0; i < MYSIZE; i++) {
            if (readyQ->ioState[i] == ACCESSING_DATABUS) {
                count++;
            }
        }
        // CALCULATES THE DATA TRANSFER SPEED DEPENDING ON WHETHER THE CALL TO THIS FUNCTION WAS MADE BY A READ OR A WRITE
        int index;
        double readSpeed, writeSpeed, readWriteSize, whichSpeed;
        readWriteSize = (double) commands[pid].syslist[sys].size; 
        for (int i = 0; i <= MAX_DEVICES; i++) {
            if (strcmp(commands[pid].syslist[sys].device, devices[i].name) == 0) {
                readSpeed = devices[i].readSpeed; 
                writeSpeed = devices[i].writeSpeed;
                index = i;
            }
        }
        if (readWrite == READCOM) {
            whichSpeed = readSpeed;\
        } else if (readWrite == WRITECOM) {
            whichSpeed = writeSpeed;\
        }
        double readWriteTime = (readWriteSize / whichSpeed * 1000000.00);
        // CALCULATES THE TIME THAT THE SYSCALL WILL BE UNBLOCKED AFTER THE DATA TRANSFER
        int time = timer + TIME_ACQUIRE_BUS + roundUp(readWriteTime);
        // IF COUNT == 0, THERE ARE NO SYSCALLS ACCESSING THE DATABUS
        if (count == 0) {
            // ADDS THE SYSCALL TO THE BLOCKED/READY QUEUE, CURRENTLY ACCESSING THE DATABUS
            enqueue(readyQ, pid, sys, time, IONUM, comTime, parentid, ACCESSING_DATABUS);
        } else {
            // ADDS THE SYSCALL TO THE BLOCKED/READY QUEUE, WAITING FOR THE DATABUS
            enqueue(readyQ, pid, sys, timer, IONUM, comTime, parentid, devices[index].readSpeed);
        }        
    }
    return timer;
}

// FUNCTION THAT RUNS WHEN THE SPAWN COMMAND IS CALLED, RETURNING THE TIMER
int spawnFunction(int timer, struct Queue* readyQ, int sys, int pid, int * CPUTime, int comTime, int parentid, int state) { 
    // IF THE TIME QUANTUM WILL EXPIRE BEFORE THE SYSCALL CAN BEGIN
    if (comTime >= timeQuantum) { 
        timer += timeQuantum; 
        timer += READY_TO_RUNNING; 
        timer += RUNNING_TO_READY; 
        // ADDS THE SAME SYSCALL TO THE END OF THE READY/BLOCKED QUEUE STRUCT
        enqueue(readyQ, pid, sys, timer, READYNUM, comTime - timeQuantum, parentid, state);
    } else {
        // FINDS AND STORE THE PROCESS ID OF THE PROCESS THAT THE SPAWN SYSCALL NAMED
        int newpid;
        for (int i = 0; i < MAX_COMMANDS; i++) {
            if (strcmp(commands[pid].syslist[sys].spawnCom, commands[i].name) == 0) {
                newpid = i;
            }
        }
        // ADDS THE NEW PROCESS TO THE END OF THE BLOCKED/READY QUEUE
        enqueue(readyQ, newpid, 0, timer, READYNUM, commands[newpid].syslist[0].commandTime, pid, state);

        // ADDS THE TOTAL TIME THAT THE NEW PROCESS IS ACTIVELY ON THE CPU TO THE CPU TIMER 
        int pidfinalSys = commands[newpid].numSyscalls - 1;
        *CPUTime += commands[newpid].syslist[pidfinalSys].when;
        
        timer += READY_TO_RUNNING;
        timer += RUNNING_TO_READY;
        // ADDED TIME FOR THE SYSTEM TO DECIDE THAT A SYSCALL NEEDS TO BE BLOCKED
        timer += 1;
        timer += comTime;

        // ADDS THE NEXT SYSCALL IN THE SAME PROCESS TO THE END OF THE READY/BLOCKED QUEUE
        enqueue(readyQ, pid, sys + 1, timer, READYNUM, commands[pid].syslist[sys+1].commandTime, parentid, state);
        
    }
    return timer;
}

// FUNCTION THAT RUNS WHEN THE WAIT COMMAND IS CALLED, RETURNING THE TIMER
int waitFunction(int timer, struct Queue* readyQ, int sys, int pid, int comTime, int parentid, int state) { 
    // IF THE TIME QUANTUM WILL EXPIRE BEFORE THE SYSCALL CAN BEGIN
    if (comTime >= timeQuantum) {
        timer += timeQuantum;
        timer += READY_TO_RUNNING; 
        timer += RUNNING_TO_READY;
        // ADDS THE SAME SYSCALL TO THE END OF THE READY/BLOCKED QUEUE STRUCT
        enqueue(readyQ, pid, sys, timer, READYNUM, comTime - timeQuantum, parentid, state);
    } // RUNS THE SECOND TIME THIS FUNCTION IS ACCESSED, IGNORING ACCESSES DUE TO NEW TIME QUANTUMS
    else if (comTime == -1) {
        timer += WAITING_TO_READY; 
        // ADDS THE NEXT SYSCALL IN THE SAME PROCESS TO THE END OF THE READY/BLOCKED QUEUE
        enqueue(readyQ, pid, sys + 1, timer, READYNUM, commands[pid].syslist[sys+1].commandTime, parentid, state);
    }
    else {
        timer += comTime;
        // ADDED TIME FOR THE SYSTEM TO DECIDE THAT A SYSCALL NEEDS TO BE BLOCKED
        timer += 1; 
        timer += READY_TO_RUNNING; 
        timer += RUNNING_TO_SLEEPING;
        comTime = -1;
        int count = 0;
        for (int i = 0; i < MYSIZE; i++) {
            if (readyQ->parent[i] == pid) {
                count++;
            }
        }
        // COUNT == 0 IF THERE ARE NO CHILD PROCESSES WAITING
        if (count == 0) {
            // ADDS THE NEXT SYSCALL IN THE SAME PROCESS TO THE END OF THE READY/BLOCKED QUEUE
            enqueue(readyQ, pid, sys + 1, timer, READYNUM, commands[pid].syslist[sys+1].commandTime, parentid, state);
        } else {
            // ADDS THE SAME SYSCALL TO THE WAITING ARRAY IN THE BLOCKED/READY QUEUE
            enqueue(readyQ, pid, sys, timer, WAITNUM, comTime, parentid, state);
        }
    }
    return timer;
}

// FUNCTION THAT RUNS TO EXECUTE THE COMMANDS FROM COMMANDS.TXT
void execute_commands(double * totalTime, double * timeOnCPU) {
    // CREATE THE QUEUE STRUCT AND CALL INITIALISEQUEUE TO INITIALISE IT
    struct Queue readyQ;
    initialiseQueue(&readyQ);

    int sys, pid, comTime, parentid, state, runningProcesses = 1, timer = 0, CPUTime = 0;
    
    // ADD THE FIRST SYSCALL IN THE FIRST PROCESS TO THE READY QUEUE
    enqueue(&readyQ, 0, 0, 0, READYNUM, commands[0].syslist[0].commandTime, -1, -1);

    while (runningProcesses > 0) {
        int timer1 = timer;
        timer = dequeue(&readyQ, &pid, &sys, timer, &comTime, &parentid, &state);
        int timer2 = timer;

        // FOR EACH SYSCALL, RUNS THE FUNCTION BASED ON THE TYPE OF SYSCALL
        if (strcmp(commands[pid].syslist[sys].syscallName, "sleep") == 0) {
            if (timer1 != timer2) {
                timer++;
            }
            timer = sleepFunction(timer, &readyQ, sys, pid, comTime, parentid, state);
        }
        
        else if (strcmp(commands[pid].syslist[sys].syscallName, "exit") == 0) {
            timer = exitFunction(timer, &readyQ, sys, pid, comTime, parentid, state);
            // DECREMENTS RUNNINGPROCESSES TO INDICATE THAT A PROCESS HAS TERMINATED
            if (comTime < timeQuantum) {
                runningProcesses--;
            }
        } 

        else if (strcmp(commands[pid].syslist[sys].syscallName, "read") == 0) {
            timer = readWriteFunction(timer, &readyQ, sys, pid, comTime, parentid, state, READCOM); 
        }

        else if (strcmp(commands[pid].syslist[sys].syscallName, "write") == 0) {
            timer = readWriteFunction(timer, &readyQ, sys, pid, comTime, parentid, state, WRITECOM);
        }

        else if (strcmp(commands[pid].syslist[sys].syscallName, "wait") == 0) {
            timer = waitFunction(timer, &readyQ, sys, pid, comTime, parentid, state);
        }        
        
        else if ((strcmp(commands[pid].syslist[sys].syscallName, "spawn") == 0)) {
            timer = spawnFunction(timer, &readyQ, sys, pid, &CPUTime, comTime, parentid, state);
            // INCREMENTS RUNNINGPROCESSES TO INDICATE THAT A NEW PROCESS HAS BEEN SPAWNED
            if (comTime < timeQuantum) {
                runningProcesses++;
            }
        }
    }

    // FINDS THE CPU TIME OF THE FIRST PROCESS IN THE COMMANDS.TXT FILE, AND ADDS THAT TIME TO CPU TIME
    int finalSys = commands[0].numSyscalls - 1; 
    CPUTime += commands[0].syslist[finalSys].when; 

    // SAVES THE TIMER POINTERS, TO BE ACCESSED IN MAIN()
    *totalTime = timer;
    *timeOnCPU = CPUTime;
}

//  ----------------------------------------------------------------------

int main(int argc, char *argv[]) {
    // ENSURE THAT WE HAVE THE CORRECT NUMBER OF COMMAND-LINE ARGUMENTS
    if(argc != 3) {
        printf("Usage: %s sysconfig-file command-file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // READ THE SYSTEM CONFIGURATION FILE
    read_sysconfig(argv[0], argv[1]);

    // READ THE COMMAND FILE
    read_commands(argv[0], argv[2]);

    // INITIALISE VARIABLES FOR MEASUREMENTS
    double totalTime = 0, timeOnCPU = 0;

    // EXECUTE COMMANDS, STARTING AT FIRST IN command-file, UNTIL NONE REMAIN
    execute_commands(&totalTime, &timeOnCPU);

    // CALCULATE THE PERCENTAGE VALUE TO DISPLAY
    int percentage = timeOnCPU / totalTime * 100;

    // PRINT THE PROGRAM'S RESULTS
    printf("measurements  %.i  %i\n", (int) totalTime, percentage);

    exit(EXIT_SUCCESS);
    return 0;
}

//  vim: ts=8 sw=4