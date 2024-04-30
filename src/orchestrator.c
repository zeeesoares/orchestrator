#include "orchestrator.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {

    if (argc != 4) {
        fprintf(stderr, "Usage: %s output_folder parallel-tasks sched-policy");
        exit(EXIT_FAILURE);
    }

    int parallel_tasks = atoi(argv[2]);

    SCHED_POL sched_pol = getSchedPol(argv[3]);

    int program_counter = 0;

    int fdRD, fdWR;
    PROCESS_REQUESTS* pr = init_process_requests(MAX_REQUESTS);
        
    write(STDOUT_FILENO,"Orchestrator is running...\n",28);
    
    unlink(MAIN_FIFO_SERVER);
    make_fifo(MAIN_FIFO_SERVER);

    open_fifo(&fdRD, MAIN_FIFO_SERVER, O_RDWR); // Abrir o FIFO para leitura
    open_fifo(&fdWR, MAIN_FIFO_SERVER, O_WRONLY); // Abrir o FIFO para escrita

    while (1) {
        PROCESS_STRUCT new;
        ssize_t read_bytes;
        read_bytes = read(fdRD, &new, sizeof(PROCESS_STRUCT));
        if (read_bytes > 0) {
            if (new.request == EXIT) break;
            else if (new.request == STATUS) print_all_process_requests(pr); 
                //handle_request_status();
            else {
                //handle_request(pr, &new);
                
            }
        }
    }
    
    close(fdRD);
    close(fdWR);
    unlink(MAIN_FIFO_SERVER);

    free_process_requests(pr);

    return 0;

}
