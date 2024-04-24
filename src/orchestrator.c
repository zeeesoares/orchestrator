#include "orchestrator.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    int fdRD, fdWR;
    PROCESS_REQUESTS* pr = init_process_requests(MAX_REQUESTS);
    fprintf(stderr,"Orchestrator is running...\n",28);
    
    unlink(MAIN_FIFO_SERVER);
    make_fifo(MAIN_FIFO_SERVER);
    
    open_fifo(&fdRD, MAIN_FIFO_SERVER, O_RDWR); // Abrir o FIFO para leitura e escrita
    open_fifo(&fdWR, MAIN_FIFO_SERVER, O_WRONLY);

    pid_t pid = fork();
    if (pid == -1) {
        perror("Erro ao criar processo filho");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { //Código Processo-Filho
        while (1) {
            PROCESS_STRUCT new;
            ssize_t read_bytes;
            read_bytes = read(fdRD, &new, sizeof(PROCESS_STRUCT));
            if (read_bytes > 0) {
                if (new.time == -1) break;
                else {
                    add_process_request(pr, &new);
                    //exec_command(new.command);
                }
            }
        }
        print_all_process_requests(pr);
        exit(EXIT_SUCCESS);
    } else { //Código Processo-Pai

        wait(NULL);
        
        print_all_process_requests(pr);

        close(fdRD);
        close(fdWR);
        unlink(MAIN_FIFO_SERVER);

        free_process_requests(pr);

        return 0;
    }
}
