#include "controller.h"
#include "utils.h"

int parse_pipeline(char *pipeline, char **commands) {
    int num_commands = 0;
    char *token;

    while ((token = strsep(&pipeline, "|")) != NULL) {
        if (*token != '\0') {
            commands[num_commands] = strdup(token);
            if (commands[num_commands] == NULL) {
                perror("Erro ao alocar memória");
                exit(EXIT_FAILURE);
            }
            num_commands++;
        }
    }
    return num_commands;
}


int exec_command(int id, char* arg, char* output_folder) {

    char *exec_args[MAX_COMMANDS + 1]; // +1 for NULL terminator
    int exec_ret;
    int i = 0;

    char* command = strdup(arg);
    char* token;

    token = strsep(&command, " ");
    while (token != NULL && i < MAX_COMMANDS) {
        exec_args[i] = token;
        i++;
        token = strsep(&command, " ");
    }
    exec_args[i] = NULL;


    char filename[256];
    snprintf(filename, sizeof(filename), "%s/task_output_%d.txt", output_folder, id);
        
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);

    exec_ret = execvp(exec_args[0], exec_args);

    return exec_ret;
}


int exec_command_pipe(char* arg){

	//Estamos a assumir numero maximo de argumentos
	char *exec_args[10];

	char *string;	
	int exec_ret = 0;
	int i=0;

	char* command = strdup(arg);

	string=strtok(command," ");
	
	while(string!=NULL){
		exec_args[i]=string;
		string=strtok(NULL," ");
		i++;
	}

	exec_args[i]=NULL;
	
	exec_ret=execvp(exec_args[0],exec_args);
	
	return exec_ret;
}

int exec_pipeline(int id, char* arg, char* output_folder) {
    int status;
    char* commands[MAX_COMMANDS];
    int number_of_commands = parse_pipeline(arg, commands);

    int number_of_pipes = number_of_commands - 1;
    pid_t pids[number_of_commands];
    int pipes[number_of_pipes][2];

    char filename[256];
    snprintf(filename, sizeof(filename), "%s/task_output_%d.txt", output_folder, id);
        
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    
    for (int i = 0; i < number_of_commands; i++) {
        if (i < number_of_pipes) pipe(pipes[i]);
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("Erro ao criar processo filho");
            exit(EXIT_FAILURE);
        } else if (pids[i] == 0) {
            // Processo filho
            if (i == 0) {
                close(pipes[i][0]);
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][1]);
                exec_command_pipe(commands[i]);
                _exit(EXIT_SUCCESS); // Termina a execução do processo filho
            } else if (i == number_of_commands - 1) {
                close(pipes[i - 1][1]);
                dup2(pipes[i - 1][0], STDIN_FILENO);
                close(pipes[i - 1][0]);
                dup2(fd, 1);
                dup2(fd, 2);
                close(fd);
                exec_command_pipe(commands[i]);
                _exit(EXIT_SUCCESS); // Termina a execução do processo filho
            } else {
                dup2(pipes[i - 1][0], STDIN_FILENO);
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i - 1][0]);
                close(pipes[i][1]);
                close(pipes[i][0]);
                exec_command_pipe(commands[i]);
                _exit(EXIT_SUCCESS); // Termina a execução do processo filho
            }
        } else {
            if (i == 0) {
                close(pipes[i][1]);
            } else if (i == number_of_commands - 1) {
                close(pipes[i - 1][0]);
            } else {
                close(pipes[i - 1][0]);
                close(pipes[i][1]);
            }
        }
    }

    for (int i = 0; i < number_of_commands; i++) {
        waitpid(pids[i], &status, 0);
    }

    return 0;
}


void dispatch(PROCESS_STRUCT* process, int fdOrchestrator, char* output_folder, int (*exec_function)(int, char*, char*)) {
    pid_t pid_fork = fork();
    

    if (pid_fork < 0) {
        perror("Erro ao criar processo filho");
        exit(EXIT_FAILURE);
    } else if (pid_fork == 0) {
        
        int exec_ret = exec_function(process->id, process->command, output_folder);
        _exit(exec_ret);
    }

    wait(NULL);


    PROCESS_STRUCT* new_process = (PROCESS_STRUCT*)malloc(sizeof(PROCESS_STRUCT));
    if (new_process == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a nova solicitação de processo\n");
        exit(EXIT_FAILURE);
    }

    memcpy(new_process, process, sizeof(PROCESS_STRUCT));
    new_process->request = WAIT;

    gettimeofday(&new_process->end_time, NULL);
    timeval_subtract(&new_process->total_time, &new_process->start_time,  &new_process->end_time);

    write(fdOrchestrator,new_process,sizeof(PROCESS_STRUCT));
    
}


void handle_tasks(
    TaskLists* tasks, int parallel_tasks, PROCESS_STRUCT* new,
    int* num_process_running, int fdOrchestrator, int fd_result,
    char* output_folder, void (*enqueue_func)(Tasks*, PROCESS_STRUCT*)) {

    switch (new->request) {
        case NEW:
            if (*num_process_running < parallel_tasks) {
                if (isEmpty(tasks->scheduled_tasks)) {
                    int pid = fork();
                    if (pid == 0) {
                        dispatch(new, fdOrchestrator, output_folder, exec_command);
                        _exit(0);
                    }
                    enqueue_func(tasks->running_tasks,new);
                    (*num_process_running) += 1;
                }
                else {
                    enqueue_func(tasks->scheduled_tasks, new);
                    PROCESS_STRUCT* nextTask = dequeue(tasks->scheduled_tasks);
                    int pid = fork();
                    if (pid == 0) {
                        dispatch(nextTask, fdOrchestrator, output_folder, exec_command);
                        _exit(0);
                    }
                    enqueue_func(tasks->running_tasks,nextTask);
                    (*num_process_running) += 1;
                }
            } else {
                enqueue_func(tasks->scheduled_tasks, new);
            }
            break;
        case PIPELINE:
            if (*num_process_running < parallel_tasks) {
                if (isEmpty(tasks->scheduled_tasks)) {
                    int pid = fork();
                    if (pid == 0) {
                        dispatch(new, fdOrchestrator, output_folder,exec_pipeline);
                        _exit(0);
                    }
                    enqueue_func(tasks->running_tasks,new);
                    (*num_process_running) += 1;
                }
                else {
                    enqueue_func(tasks->scheduled_tasks, new);
                    PROCESS_STRUCT* nextTask = dequeue(tasks->scheduled_tasks);
                    int pid = fork();
                    if (pid == 0) {
                        dispatch(nextTask, fdOrchestrator, output_folder,exec_pipeline);
                        _exit(0);
                    }
                    enqueue_func(tasks->running_tasks,nextTask);
                    (*num_process_running) += 1;
                }
            } else {
                enqueue_func(tasks->scheduled_tasks, new);
            }
            break;

        case WAIT:
            wait(NULL);
            PROCESS_STRUCT* completedTask = dequeueById(tasks->running_tasks, new->id);

            char completed_message[512];

    
            snprintf(completed_message, sizeof(completed_message), "%d %s %.2f ms\n", completedTask->id, completedTask->command, timeval_to_ms(&completedTask->total_time));

            write(fd_result, completed_message, strlen(completed_message));

            enqueue_func(tasks->completed_tasks, completedTask);
            (*num_process_running) -= 1;
            if (!isEmpty(tasks->scheduled_tasks)) {
                PROCESS_STRUCT* process = dequeue(tasks->scheduled_tasks);
                int pid = fork();
                if (pid == 0) {
                    if (process->request == NEW) {
                        dispatch(process, fdOrchestrator, output_folder,exec_command); 
                    } else {
                        dispatch(process, fdOrchestrator, output_folder,exec_pipeline); 
                    }
                    _exit(0);
                }
                enqueue_func(tasks->running_tasks, process);
                (*num_process_running)++;
            }
            break;
        case WAITSTATUS:
            int status = 0;
            waitpid(new->pid,&status,0);
            break;
    }
}

Tasks* createLinkedList() {
    Tasks* list = (Tasks*)malloc(sizeof(Tasks));
    if (list == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a lista ligada\n");
        exit(EXIT_FAILURE);
    }
    list->head = NULL;
    list->tail = NULL;
    return list;
}


int isEmpty(Tasks* list) {
    return list->head == NULL;
}


void enqueue(Tasks* list, PROCESS_STRUCT* process) {
    Task* newTask = (Task*)malloc(sizeof(Task));
    if (newTask == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o novo nó\n");
        exit(EXIT_FAILURE);
    }

    PROCESS_STRUCT* new_process = (PROCESS_STRUCT*)malloc(sizeof(PROCESS_STRUCT));
    if (new_process == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a nova solicitação de processo\n");
        exit(EXIT_FAILURE);
    }
        
    memcpy(new_process, process, sizeof(PROCESS_STRUCT));
    newTask->process = new_process;
    newTask->next = NULL;

    if (list->tail == NULL) {
        list->head = newTask;
        list->tail = newTask;
    } else {
        list->tail->next = newTask;
        list->tail = newTask;
    }
}



void enqueueSorted(Tasks* list, PROCESS_STRUCT* process) {
    Task* newTask = (Task*)malloc(sizeof(Task));
    if (newTask == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o novo nó\n");
        exit(EXIT_FAILURE);
    }
    PROCESS_STRUCT* new_process = (PROCESS_STRUCT*)malloc(sizeof(PROCESS_STRUCT));
    if (new_process == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a nova solicitação de processo\n");
        exit(EXIT_FAILURE);
    }
        
    memcpy(new_process, process, sizeof(PROCESS_STRUCT));
    newTask->process = new_process;
    newTask->next = NULL;

    if (list->head == NULL || list->head->process->time > process->time) {
        newTask->next = list->head;
        list->head = newTask;
        if (list->tail == NULL) {
            list->tail = newTask;
        }
        return;
    }

    Task* current = list->head;
    // Move para o próximo nó com tempo igual ou maior
    while (current->next != NULL && current->next->process->time <= process->time) {
        current = current->next;
    }

    newTask->next = current->next;
    current->next = newTask;
    if (newTask->next == NULL) {
        list->tail = newTask;
    }
}


PROCESS_STRUCT* dequeue(Tasks* list) {
    if (list->head == NULL) {
        return NULL;
    }
    Task* temp = list->head;
    PROCESS_STRUCT* process = temp->process;
    list->head = temp->next;
    if (list->head == NULL) {
        list->tail = NULL;
    }
    free(temp);
    return process;
}


PROCESS_STRUCT* dequeueById(Tasks* list, int id) {
    if (list->head == NULL) {
        return NULL;
    }

    Task* current = list->head;
    Task* prev = NULL;

    while (current != NULL && current->process->id != id) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        return NULL;
    }

    if (prev != NULL) {
        prev->next = current->next;
    } else {
        list->head = current->next;
    }

    if (current == list->tail) {
        list->tail = prev;
    }

    PROCESS_STRUCT* process = current->process;
    free(current);

    return process;
}


void freeTasks(Tasks* list) {
    Task* current = list->head;
    while (current != NULL) {
        Task* next = current->next;
        free(current->process);
        free(current);
        current = next;
    }
    free(list);
}


void printLinkedList(Tasks* list) {
    if (list->head == NULL) {
        printf("A lista está vazia.\n");
        return;
    }

    Task* current = list->head;
    printf("Processos na lista ligada:\n");
    while (current != NULL) {
        printf("ID: %d, Comando: %s\n", current->process->id, current->process->command);
        current = current->next;
    }
}

void sendProcessState(int fdClient, int id, const char* command) {
    char message[MAX_RESPONSE_SIZE];
    snprintf(message, sizeof(message), "%d %s\n", id, command);
    write(fdClient, message, strlen(message));
}


void sendLinkedList(Tasks* list, int fdClient) {
    if (list->head == NULL) {
        char message[MAX_RESPONSE_SIZE];
        snprintf(message, sizeof(message), "A lista está vazia.\n");
        write(fdClient, message, strlen(message));
        return;
    }

    Task* current = list->head;
    while (current != NULL) {
        sendProcessState(fdClient, current->process->id, current->process->command);
        current = current->next;
    }
}

void handle_status_request(int fdClient, int fdOrchestrator, Tasks* running_tasks, Tasks* scheduled_tasks, Tasks* completed_tasks) {
    int fork_status = fork();

    if (fork_status == 0) {
        write(fdClient,"Executing:\n",11);
        sendLinkedList(running_tasks, fdClient);

        write(fdClient,"\nScheduled:\n",13);
        sendLinkedList(scheduled_tasks, fdClient);

        write(fdClient,"\nCompleted:\n",13);
        sendLinkedList(completed_tasks, fdClient);

        _exit(0);
    }
    wait(NULL);
    PROCESS_STRUCT* new_process = (PROCESS_STRUCT*)malloc(sizeof(PROCESS_STRUCT));
    if (new_process == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a nova solicitação de processo\n");
        exit(EXIT_FAILURE);
    }

    new_process->request = WAITSTATUS;
    new_process->pid = getpid();

    write(fdOrchestrator,new_process,sizeof(PROCESS_STRUCT));
}
