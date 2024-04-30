#include "utils.h"


void make_fifo(char *fifo_name) {
    if (mkfifo(fifo_name, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
}

char *create_fifo(pid_t pid) {
    char *fifo_name = malloc(sizeof(char) * 64);
    sprintf(fifo_name, "tmp/FIFO_%d", pid);  // NOLINT

    if (mkfifo(fifo_name, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    return fifo_name;
}

char *create_fifo_name(pid_t pid) {
    char *fifo_name = malloc(sizeof(char) * 64);
    sprintf(fifo_name, "tmp/FIFO_%d", pid);  // NOLINT
    return fifo_name;
}

void open_fifo(int *fd, char *fifo_name, int flags) {
    *fd = open(fifo_name, flags);

    if (*fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
}

SCHED_POL getSchedPol(char* arg) {
    if (!strcmp(arg,"FCFS"))
        return FCFS;
    else if (!strcmp(arg,"SJF"))
        return SJF;
    else 
        exit(EXIT_FAILURE);
}

REQUEST_TYPE getRequestType(char* arg) {
        if (!strcmp(arg,"execute"))
        return EXECUTE;
    else if (!strcmp(arg,"status"))
        return STATUS;
    else if (!strcmp(arg,"exit"))
        return EXIT;
    else 
        exit(EXIT_FAILURE);
}