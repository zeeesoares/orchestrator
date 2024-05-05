#ifndef UTILS_H
#define UTILS_H

#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include "controller.h"

#define MAIN_FIFO_SERVER "tmp/FIFO_SERVER"

void make_fifo(char *fifo_name);

char *create_fifo(pid_t pid);

char *create_fifo_name(pid_t pid);

void send_client_response(int pid, int request_id);

void open_fifo(int *fd, char *fifo_name, int flags);

double timeval_diff(struct timeval *start, struct timeval *end);

long timeval_diff_milliseconds(struct timeval *start, struct timeval *end);

const char* getRequestType(REQUEST_TYPE req);

SCHED_POL getSchedPol(char* arg);

#endif