#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>

#define MAX_BUFFER_SIZE 300

#define MAX_REQUESTS 100

typedef struct PROCESS_STRUCT {
    int pid;
    int time;
    char command[MAX_BUFFER_SIZE - sizeof(int) * 2];
} PROCESS_STRUCT;

typedef struct PROCESS_REQUESTS {
    PROCESS_STRUCT** requests;
    int current_index;
    int capacity;
} PROCESS_REQUESTS;

typedef enum request_type {
  NEW,
  EXECUTING,
  DONE
} REQUEST_TYPE;



PROCESS_REQUESTS* init_process_requests(int capacity);

void add_process_request(PROCESS_REQUESTS* pr, PROCESS_STRUCT* process);

void print_all_process_requests(PROCESS_REQUESTS* pr);

void free_process_requests(PROCESS_REQUESTS* pr);

#endif 