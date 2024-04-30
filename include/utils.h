#ifndef UTILS_H
#define UTILS_H

#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAIN_FIFO_SERVER "tmp/FIFO_SERVER"

#define MAX_BUFFER_SIZE 300

typedef enum SCHED_POL {
    FCFS,
    SJF
} SCHED_POL;

typedef enum REQUEST_TYPE {
    EXECUTE,
    STATUS,
    EXIT
} REQUEST_TYPE;

typedef struct task {
    int id;
    pid_t pid;
    int time;
    REQUEST_TYPE type;
    char command[MAX_BUFFER_SIZE - sizeof(int) * 2];;
} Task;

void make_fifo(char *fifo_name);

char *create_fifo(pid_t pid);

char *create_fifo_name(pid_t pid);

void open_fifo(int *fd, char *fifo_name, int flags);

SCHED_POL getSchedPol(char* arg);

REQUEST_TYPE getRequestType(char* arg);

#endif