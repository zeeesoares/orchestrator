#include "controller.h"
#include "utils.h"



int exec_command(int id, char* arg, int fdOrchestrator) {
    int status;

    char *exec_args[MAX_COMMANDS + 1]; // +1 for NULL terminator
    int exec_ret = 0;
    int i = 0;

    char* command = strdup(arg);
    char* token;
    char* saveptr;

    token = strsep(&command, " ");
    while (token != NULL && i < MAX_COMMANDS) {
        exec_args[i] = token;
        i++;
        token = strsep(&command, " ");
    }
    exec_args[i] = NULL;

    //MANDAR SINAL? 

    char filename[256];
    snprintf(filename, sizeof(filename), "tmp/task_output_%d.txt", id);
        
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);

    exec_ret = execvp(exec_args[0], exec_args);

    return EXIT_FAILURE;
}


void dispatch(PROCESS_STRUCT* process, int fdOrchestrator) {
    pid_t pid_fork = fork();
    

    if (pid_fork < 0) {
        // Erro ao criar o processo filho
        perror("Erro ao criar processo filho");
        exit(EXIT_FAILURE);
    } else if (pid_fork == 0) {
        
        
        int exec_ret = exec_command(process->id, process->command, fdOrchestrator);
        _exit(exec_ret); // Saída do processo filho após execução
    } 
}

void handle_tasks(Tasks* tasks, int parallel_tasks, PROCESS_STRUCT* new, int* num_process_running, int fdOrchestrator) {
    int status;
    
    switch (new->request) {
        case NEW:
            if (*num_process_running < parallel_tasks) {
                if (isEmpty(tasks)) {
                    dispatch(new, fdOrchestrator);
                    (*num_process_running)++;
                }
                else {
                    enqueue(tasks, new);
                    Task* nextTask = dequeue(tasks);
                    dispatch(nextTask->process, fdOrchestrator); 
                    (*num_process_running)++;
                }
            } else {
                enqueue(tasks, new);
            }
            break;
        case KILL:
            wait(NULL);
            (*num_process_running)--;
            Task* nextTask = dequeue(tasks);
            dispatch(nextTask->process, fdOrchestrator); 
            (*num_process_running)++;
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
        // A lista está vazia
        list->head = newTask;
        list->tail = newTask;
    } else {
        // Adiciona o novo nó após o último nó
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
        // Inserir no início se a lista estiver vazia ou se o tempo do novo processo for menor que o tempo do primeiro processo na lista
        newTask->next = list->head;
        list->head = newTask;
        if (list->tail == NULL) {
            // Se a lista estava vazia, o novo nó também é o último nó
            list->tail = newTask;
        }
        return;
    }

    // Procurar a posição correta para inserção
    Task* current = list->head;
    while (current->next != NULL && current->next->process->time < process->time) {
        current = current->next;
    }

    newTask->next = current->next; // Inserir na posição correta
    current->next = newTask;
    if (newTask->next == NULL) {
        // Se o novo nó for inserido no final da lista, ele se torna o último nó
        list->tail = newTask;
    }
}


PROCESS_STRUCT* dequeue(Tasks* list) {
    if (list->head == NULL) {
        // A lista está vazia
        return NULL;
    }
    Task* temp = list->head;
    PROCESS_STRUCT* process = temp->process;
    list->head = temp->next;
    if (list->head == NULL) {
        // A lista agora está vazia, então o tail também deve ser NULL
        list->tail = NULL;
    }
    free(temp);
    return process;
}

void freeTasks(Tasks* list) {
    Task* current = list->head;
    while (current != NULL) {
        Task* next = current->next;
        free(current->process); // Libera a memória alocada para a estrutura de processos
        free(current); // Libera a memória alocada para o nó
        current = next;
    }
    free(list); // Libera a memória alocada para a lista ligada
}

void printLinkedList(Tasks* list) {
    if (list->head == NULL) {
        printf("A lista está vazia.\n");
        return;
    }

    Task* current = list->head;
    printf("Processos na lista ligada:\n");
    while (current != NULL) {
        printf("PID: %d, Tempo: %d, Comando: %s\n", current->process->pid, current->process->time, current->process->command);
        current = current->next;
    }
}



/*
void controller(PROCESS_REQUESTS* pr, int parallelTasks) {
    int program_counter = 0;
    int executed_programs = 0;
    int programs_in_queue = pr->current_index + 1;

    while (executed_programs < programs_in_queue) {
        while (program_counter < parallelTasks && executed_programs < programs_in_queue) {
            pid_t exec_prog = fork();
            if (exec_prog == 0) {
                int res = exec_command(pr->requests[executed_programs]->pid,pr->requests[executed_programs]->command);
                _exit(EXIT_FAILURE);
            }
            executed_programs++;
            program_counter++;
            wait(NULL);
            //MANDAR SINAL?

            program_counter--;
        }
    }

    while (program_counter > 0) {
        wait(NULL);
        program_counter--;
    }
}
*/


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
