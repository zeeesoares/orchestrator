#include "utils.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "orchestrator.h"
#include "controller.h"

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
        perror("Erro ao abrir FIFO");
        exit(EXIT_FAILURE);
    }
}

double timeval_diff_microseconds(struct timeval *start, struct timeval *end) {
    double start_us = (double) start->tv_sec * 1000000.0 + (double) start->tv_usec;
    double end_us = (double) end->tv_sec * 1000000.0 + (double) end->tv_usec;
    return end_us - start_us;
}

double timeval_to_ms(struct timeval *time) {
  return (double)time->tv_sec * 1000.0 + (double)time->tv_usec / 1000.0;
}

double timeval_diff_seconds(struct timeval *start, struct timeval *end) {
    double seconds_diff = (double)(end->tv_sec - start->tv_sec);
    double microseconds_diff = (double)(end->tv_usec - start->tv_usec) / 1000000.0;
    return (seconds_diff + microseconds_diff) / 1000.0; // Convertendo para segundos
}


long timeval_diff_milliseconds(struct timeval *start, struct timeval *end) {
    long seconds_diff = (end->tv_sec - start->tv_sec) * 1000;
    long microseconds_diff = (end->tv_usec - start->tv_usec) / 1000;
    return seconds_diff + microseconds_diff;
}

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
  struct timeval xx = *x;
  struct timeval yy = *y;
  x = &xx; y = &yy;

  if (x->tv_usec > 999999)
  {
    x->tv_sec += x->tv_usec / 1000000;
    x->tv_usec %= 1000000;
  }

  if (y->tv_usec > 999999)
  {
    y->tv_sec += y->tv_usec / 1000000;
    y->tv_usec %= 1000000;
  }

  result->tv_sec = x->tv_sec - y->tv_sec;

  if ((result->tv_usec = x->tv_usec - y->tv_usec) < 0)
  {
    result->tv_usec += 1000000;
    result->tv_sec--; // borrow
  }

  return result->tv_sec < 0;
}



void send_client_response(int pid, int request_id) {
    int fd;

    char* fifoName = create_fifo_name(pid);
    open_fifo(&fd, fifoName, O_WRONLY);
    
    write(fd, &request_id, sizeof(int));
    close(fd);
}

const char* getRequestType(REQUEST_TYPE req) {
    switch (req) 
    {
        case NEW: return "New";
        case STATUS: return "Status";
        case PIPELINE: return "Pipeline";
        case EXIT: return "Exit";
        default:  return "Null";
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