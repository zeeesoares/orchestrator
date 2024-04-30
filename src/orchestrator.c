#include "orchestrator.h"
#include "utils.h"


int main(int argc, char *argv[]) {

    if (argc != 4) {
        fprintf(stderr, "Usage: %s output_folder parallel-tasks sched-policy");
        exit(EXIT_FAILURE);
    }

    int parallel_tasks = atoi(argv[2]);

    SCHED_POL sched_pol = getSchedPol(argv[3]);

    int program_counter = 0;

    int fdRD, fdWR;
        
    write(STDOUT_FILENO,"Orchestrator is running...\n",28);
    
    unlink(MAIN_FIFO_SERVER);
    make_fifo(MAIN_FIFO_SERVER);

    open_fifo(&fdRD, MAIN_FIFO_SERVER, O_RDWR); // Abrir o FIFO para leitura
    open_fifo(&fdWR, MAIN_FIFO_SERVER, O_WRONLY); // Abrir o FIFO para escrita

    while (1) {
        Task new;
        ssize_t read_bytes;
        read_bytes = read(fdRD, &new, sizeof(Task));
        if (read_bytes > 0) {
            if (new.type == EXIT) break;
            else if (new.type == STATUS) break; 
                //handle_request_status()
            else {
                program_counter++;
                char* clientFIFO = create_fifo_name(new.pid);
                int fdClientRet;
                open_fifo(&fdClientRet,clientFIFO,O_WRONLY);
                write(fdClientRet,program_counter,sizeof(int));
            }
        }
    }
    
    close(fdRD);
    close(fdWR);
    unlink(MAIN_FIFO_SERVER);


    return 0;

}
