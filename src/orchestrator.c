#include "orchestrator.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "controller.h"


int main(int argc, char *argv[]) {

    if (argc != 4) {
        fprintf(stderr, "Usage: %s output_folder parallel-tasks sched-policy", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* output_folder = argv[1];
    int parallel_tasks = atoi(argv[2]);

    SCHED_POL sched_pol = getSchedPol(argv[3]);

    int program_counter = 0;

    int fdRD, fdWR;

    char filename[256];
    snprintf(filename, sizeof(filename), "%s/%s", output_folder, COMPLETED_TASKS_FILE);
    int fd_results = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_results == -1) {
        perror("Erro ao abrir ficheiro completed_tasks, insira um output_folder válido!");
        exit(EXIT_FAILURE);
    }

    TaskLists task_lists;
    task_lists.running_tasks = createLinkedList();
    task_lists.scheduled_tasks = createLinkedList();
    task_lists.completed_tasks = createLinkedList();

    write(STDOUT_FILENO, "Orchestrator is running...\n", 28);
    
    unlink(MAIN_FIFO_SERVER);
    make_fifo(MAIN_FIFO_SERVER);

    open_fifo(&fdRD, MAIN_FIFO_SERVER, O_RDWR); // Abrir o FIFO para leitura
    open_fifo(&fdWR, MAIN_FIFO_SERVER, O_WRONLY); // Abrir o FIFO para escrita

    int num_process_running = 0;

    // Define a função de enqueue com base na política de escalonamento
    void (*enqueue_func)(Tasks*, PROCESS_STRUCT*);
    if (sched_pol == FCFS) {
        enqueue_func = enqueue;
    } else {
        enqueue_func = enqueueSorted;
    }

    while (1) {
        PROCESS_STRUCT new;
        ssize_t read_bytes;
        read_bytes = read(fdRD, &new, sizeof(PROCESS_STRUCT));
        if (read_bytes > 0) {
            if (new.request == EXIT) break;
            else if (new.request == STATUS) {
                int fork_id = fork();
                if (fork_id == 0) {
                    int fdClient;
                    char* clientFIFO = create_fifo_name(new.pid);
                    open_fifo(&fdClient,clientFIFO, O_WRONLY);
                    handle_status_request(fdClient,task_lists.running_tasks,task_lists.scheduled_tasks,task_lists.completed_tasks);
                    close(fdClient);
                    _exit(0);
                }
            }
            else {
                if (new.request == NEW) {
                    program_counter++;
                    printf("%d\n", program_counter);
                    new.id = program_counter;
                    send_client_response(new.pid,new.id);
                    gettimeofday(&new.start_time, NULL);
                }
                handle_tasks(&task_lists, parallel_tasks, &new, &num_process_running, fdWR, fd_results, output_folder, enqueue_func);
            }
        }
    }
    
    close(fdRD);
    close(fdWR);
    
    unlink(MAIN_FIFO_SERVER);

    freeTasks(task_lists.running_tasks);
    freeTasks(task_lists.scheduled_tasks);
    freeTasks(task_lists.completed_tasks);

    return 0;

}
