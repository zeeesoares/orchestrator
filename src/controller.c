#include "controller.h"
#include "utils.h"



int exec_command(int id, char* arg, int fdOrchestrator) {

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
    snprintf(filename, sizeof(filename), "tmp/task_output_%d.txt", id);
        
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);

    exec_ret = execvp(exec_args[0], exec_args);

    return exec_ret;
}


void dispatch(PROCESS_STRUCT* process, int fdOrchestrator) {
    pid_t pid_fork = fork();
    

    if (pid_fork < 0) {
        perror("Erro ao criar processo filho");
        exit(EXIT_FAILURE);
    } else if (pid_fork == 0) {
        
        int exec_ret = exec_command(process->id, process->command, fdOrchestrator);
        _exit(exec_ret);
    }

    wait(NULL);

    gettimeofday(&process->end_time,NULL);

    PROCESS_STRUCT* new_process = (PROCESS_STRUCT*)malloc(sizeof(PROCESS_STRUCT));
    if (new_process == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a nova solicitação de processo\n");
        exit(EXIT_FAILURE);
    }
        
    new_process->request = WAIT;
    new_process->pid = getpid();

    write(fdOrchestrator,new_process,sizeof(PROCESS_STRUCT));
    
}

void handle_tasks(Tasks* tasks, int parallel_tasks, PROCESS_STRUCT* new, int* num_process_running, int fdOrchestrator) {

    switch (new->request) {
        case NEW:
            send_client_response(new->pid,new->id);
            if (*num_process_running < parallel_tasks) {
                if (isEmpty(tasks)) {
                    int pid = fork();
                    if (pid == 0) {
                        dispatch(new, fdOrchestrator);
                        _exit(0);
                    }
                    (*num_process_running) += 1;
                }
                else {
                    enqueue(tasks, new);
                    Task* nextTask = dequeue(tasks);
                    int pid = fork();
                    if (pid == 0) {
                        dispatch(nextTask->process, fdOrchestrator);
                        _exit(0);
                    }
                    (*num_process_running) += 1;
                }
            } else {
                enqueue(tasks, new);
            }
            break;
        case WAIT:
            wait(NULL);
            (*num_process_running) -= 1;
            if (!isEmpty(tasks)) {
                PROCESS_STRUCT* process = dequeue(tasks);
                dispatch(process, fdOrchestrator); 
                (*num_process_running)++;
            }
            break;
        default:
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
    while (current->next != NULL && current->next->process->time < process->time) {
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
        printf("ID: %d, Tempo: %d, Comando: %s\n", current->process->id, current->process->time, current->process->command);
        current = current->next;
    }
}


// while (i < img) {
//     while (j < max && j < img) 
//         fork ()
//         i++
//         j--

//     wait()
//     j--
// }
// while (j > 0) {
//     wait();
//     j--;
// }
