#include "controller.h"
#include "utils.h"


int exec_command(int pid, char* arg) {
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

    pid_t pid_fork = fork();

    if (pid_fork == 0) {
        char filename[256];
        snprintf(filename, sizeof(filename), "tmp/task_output_%d.txt", pid);
        
        int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(fd, 1);
        dup2(fd, 2);
        close(fd);

        exec_ret = execvp(exec_args[0], exec_args);
        _exit(0);
    }
    wait(NULL);

    if (WEXITSTATUS(status)) {
        return EXIT_SUCCESS;
    }
    
    return EXIT_FAILURE;
}

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
        perror("Capacidade maxima excedida");
        exit(EXIT_FAILURE);
    }
}

void handle_request(PROCESS_REQUESTS* pr, PROCESS_STRUCT* process) {
    add_process_request(pr,process);
    if (1) schedule_request(pr);
    controller(pr,4);
}


int compare_time(const void *a, const void *b) {
    const PROCESS_STRUCT *pa = *(const PROCESS_STRUCT **)a;
    const PROCESS_STRUCT *pb = *(const PROCESS_STRUCT **)b;
    
    return pa->time - pb->time;
}

void schedule_request(PROCESS_REQUESTS* pr) {
    int size = pr->current_index;
    qsort(pr->requests, size, sizeof(PROCESS_STRUCT *), compare_time);
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
