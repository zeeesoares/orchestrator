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
#include <sys/wait.h>


#define MAX_BUFFER_SIZE 300
#define MAX_COMMANDS 10
#define MAX_REQUESTS 100

typedef enum request_type {
  COMMAND,
  NEW,
  STATUS,
  WAITING,
  RUNNING,
  FINISHED,
  WAIT,
  PIPELINE,
  EXIT
} REQUEST_TYPE;

typedef struct PROCESS_STRUCT {
    int pid;
    int id;
    int time;
    struct timeval start_time, end_time;
    REQUEST_TYPE request;
    char command[MAX_BUFFER_SIZE - sizeof(int) * 2];
} PROCESS_STRUCT;

typedef struct Task {
    PROCESS_STRUCT* process;
    struct Task* next;
} Task;

typedef struct Tasks {
    Task* head;
    Task* tail;
} Tasks;

int exec_command(int id, char* arg, int fdOrchestrator);

void dispatch(PROCESS_STRUCT* process, int fdOrchestrator);

void handle_tasks(Tasks* tasks, int parallel_tasks, PROCESS_STRUCT* new, int* num_process_running, int fdOrchestrator);

Tasks* createLinkedList();

int isEmpty(Tasks* list);

void enqueue(Tasks* list, PROCESS_STRUCT* process);

void enqueueSorted(Tasks* list, PROCESS_STRUCT* process);

PROCESS_STRUCT* dequeue(Tasks* list);

void freeTasks(Tasks* list);

void printLinkedList(Tasks* list);


#endif 