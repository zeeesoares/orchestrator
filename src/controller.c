#include "controller.h"
#include <stdio.h>

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

void add_process_request(PROCESS_REQUESTS* pr, PROCESS_STRUCT* process) {
    if (pr->current_index >= pr->capacity) {
        fprintf(stderr, "Capacidade máxima de solicitações de processo atingida\n");
        return;
    }
    pr->requests[pr->current_index] = process;
    pr->current_index++;
}

void print_all_process_requests(PROCESS_REQUESTS* pr) {
    printf("Processos armazenados:\n");
    for (int i = 0; i < pr->current_index; i++) {
        PROCESS_STRUCT* process = pr->requests[i];
        printf("PID: %d, Time: %d, Command: %s\n", process->pid, process->time, process->command);
    }
}

void free_process_requests(PROCESS_REQUESTS* pr) {
    for (int i = 0; i < pr->current_index; i++) {
        free(pr->requests[i]);
    }
    free(pr->requests);
    free(pr);
}
