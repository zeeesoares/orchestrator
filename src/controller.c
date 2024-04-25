#include "controller.h"
#include "utils.h"

PROCESS_REQUESTS* init_process_requests(int capacity) {
    PROCESS_REQUESTS* pr = (PROCESS_REQUESTS*)malloc(sizeof(PROCESS_REQUESTS));
    if (pr == NULL) {
        perror("Erro ao alocar memória para a estrutura de solicitações de processo");
        exit(EXIT_FAILURE);
    }
    pr->requests = (PROCESS_STRUCT**)malloc(capacity * sizeof(PROCESS_STRUCT*));
    if (pr->requests == NULL) {
        perror("Erro ao alocar memória para a matriz de processos");
        free(pr);
        exit(EXIT_FAILURE);
    }
    pr->current_index = 0;
    pr->capacity = capacity;
    return pr;
}

void append_process_struct(PROCESS_REQUESTS* pr, PROCESS_STRUCT* process) {
    PROCESS_STRUCT* new_process = (PROCESS_STRUCT*)malloc(sizeof(PROCESS_STRUCT));
    if (new_process == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a nova solicitação de processo\n");
        exit(EXIT_FAILURE);
    }
        
    memcpy(new_process, process, sizeof(PROCESS_STRUCT));

    new_process->request = NEW;

    pr->requests[pr->current_index] = new_process;
    pr->current_index++;
}

void add_process_request(PROCESS_REQUESTS* pr, PROCESS_STRUCT* process) {
    if (pr->current_index < pr->capacity) {
        append_process_struct(pr,process);
    } else {
        pr->capacity *= 2;
        pr->requests = realloc(pr->requests, sizeof(PROCESS_STRUCT *) * pr->capacity);
        if (pr->requests == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        append_process_struct(pr,process);
    }
}

void schedule_request(PROCESS_REQUESTS* pr, PROCESS_STRUCT* process, int sched_pol) {
    

}


void print_all_process_requests(PROCESS_REQUESTS* pr) {
    printf("Processos armazenados:\n");
    for (int i = 0; i < pr->current_index; i++) {
        PROCESS_STRUCT* process = pr->requests[i];
        printf("PID: %d, Time: %d, Command: %s, STATE: %s\n", process->pid, process->time, process->command,getRequestType(process->request));
    }
}

void free_process_requests(PROCESS_REQUESTS* pr) {
    for (int i = 0; i < pr->current_index; i++) {
        free(pr->requests[i]);
    }
    free(pr->requests);
    free(pr);
}
