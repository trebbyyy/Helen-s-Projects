## Overview

- myscheduler is a program developed for the CITS2002 Systems Programming unit. It emulates the scheduling of processes on a single-CPU, multi-device system using a pre-emptive process scheduler. 

## How To Use

Compile the program using a C11 compiler. For example, using GCC:
- gcc -o myscheduler myscheduler.c

then, run 'myscheduler' with the following cmd-line args:
- ./myscheduler sysconfig-file command-file

The sysconfig-file and command-file formats are as follows:    

## sysconfig-file

#            devicename   readspeed      writespeed
#
device       usb3.1       640000000Bps   640000000Bps
device       terminal     10Bps          3000000Bps
device       hd           160000000Bps   80000000Bps
device       ssd          480000000Bps   420000000Bps
#
timequantum  100usec

## command-file: 

Note that lines commencing with a '#' are comment lines, that the 'words' on each line may be separated by one-or-more space or tab characters, and that the commands do not receive or require any commnad-line arguments.

#
shortsleep
← tab →10usecs    sleep   1000000usecs
← tab →50usecs    exit
#
longsleep
← tab →10usecs    sleep   5000000usecs
← tab →50usecs    exit
#
cal
← tab →80usecs    write   terminal 2000B
← tab →90usecs    exit
#
copyfile
← tab →200usecs   read    hd       48000B
← tab →300usecs   write   hd       48000B
← tab →600usecs   read    hd       21000B
← tab →700usecs   write   hd       21000B
← tab →1000usecs  exit
#
shell
← tab →100usecs   write   terminal 10B
← tab →120usecs   read    terminal 4B
← tab →220usecs   spawn   cal
← tab →230usecs   wait
← tab →1000usecs  exit
#


## Output

- myscheduler will output a single line with the total time taken (in microseconds) to complete the emulation and the CPU utilization (as a percentage).

**prompt> ./myscheduler  sysconfig-file  command-file**
**measurements  482400  24**

