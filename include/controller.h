#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_BUFFER_SIZE 300
#define MAX_COMMANDS 10
#define MAX_REQUESTS 100

typedef enum request_type {
  COMMAND,
  NEW,
  STATUS,
  PIPELINE,
  EXIT
} REQUEST_TYPE;

typedef struct PROCESS_STRUCT {
    int pid;
    int time;
    REQUEST_TYPE request;
    char command[MAX_BUFFER_SIZE - sizeof(int) * 2];
} PROCESS_STRUCT;

typedef struct PROCESS_REQUESTS {
    PROCESS_STRUCT** requests;
    int current_index;
    int capacity;
} PROCESS_REQUESTS;

PROCESS_REQUESTS* init_process_requests(int capacity);

void handle_request(PROCESS_REQUESTS* pr, PROCESS_STRUCT* process);

int compare_time(const void *a, const void *b);

void schedule_request(PROCESS_REQUESTS* pr);

void add_process_request(PROCESS_REQUESTS* pr, PROCESS_STRUCT* process);

void print_all_process_requests(PROCESS_REQUESTS* pr);

void free_process_requests(PROCESS_REQUESTS* pr);

#endif 