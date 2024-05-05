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
#define MAX_RESPONSE_SIZE 300

#define COMPLETED_TASKS_FILE "completed_tasks.txt"

typedef enum SCHED_POL {
    FCFS,
    SJF
} SCHED_POL;

typedef enum request_type {
  NEW,
  STATUS,
  WAIT,
  WAITSTATUS,
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

typedef struct {
    Tasks* running_tasks;
    Tasks* scheduled_tasks;
    Tasks* completed_tasks;
} TaskLists;


int parse_pipeline(char *pipeline, char **commands);

int exec_pipeline(int id, char* arg, char* output_folder);

int exec_command(int id, char* arg, char* output_folder);

void dispatch(PROCESS_STRUCT* process, int fdOrchestrator, char* output_folder);

void handle_tasks(TaskLists* tasks, int parallel_tasks, PROCESS_STRUCT* new, int* num_process_running, int fdOrchestrator, int fd_result, char* output_folder,  void (*enqueue_func)(Tasks*, PROCESS_STRUCT*));

Tasks* createLinkedList();

int isEmpty(Tasks* list);

void enqueue(Tasks* list, PROCESS_STRUCT* process);

void enqueueSorted(Tasks* list, PROCESS_STRUCT* process);

PROCESS_STRUCT* dequeue(Tasks* list);

PROCESS_STRUCT* dequeueById(Tasks* list, int id);

void freeTasks(Tasks* list);

void printLinkedList(Tasks* list);

void append_tasks_info(char* buffer, Task* current);

void create_status_buffer(char* buffer, const char* title, Tasks* list);

void status_to_buffer(char* buffer, Tasks* running_tasks, Tasks* scheduled_tasks, Tasks* completed_tasks);

void handle_status_request(int fdClient, Tasks* running_tasks, Tasks* scheduled_tasks, Tasks* completed_tasks);

#endif 