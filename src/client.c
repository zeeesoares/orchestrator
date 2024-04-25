#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>



int main(int argc, char *argv[]) {
    int fd;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <option>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* option = argv[1];

    if (!strcmp(option,"execute")) {
        if (argc < 5) {
            fprintf(stderr, "Uso: %s execute -time -u/-p \"<programa> [args]\"\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        open_fifo(&fd,MAIN_FIFO_SERVER,O_WRONLY);

        PROCESS_STRUCT new;
        new.pid = getpid();
        new.time = atoi(argv[2]);
        new.request = COMMAND;
        strcpy(new.command, argv[4]);

        char *buffername = create_fifo(new.pid);
    
        ssize_t writen_bytes;
        writen_bytes = write(fd, &new, sizeof(PROCESS_STRUCT)); 
        close(fd);

        unlink(buffername);

        if (writen_bytes > 0) {
            sprintf(buffername, "Tarefa %d - Submetida!\n", new.pid);
            write(1,buffername,MAX_BUFFER_SIZE);
        }
    }
    else if  (!strcmp(option,"exit")) {
        if (argc < 2) {
            fprintf(stderr, "Uso: %s exit\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        open_fifo(&fd,MAIN_FIFO_SERVER,O_WRONLY);

        PROCESS_STRUCT new;
        new.pid = getpid();
        new.time = 0;
        new.request = EXIT;

        char *buffername = create_fifo(new.pid);
    
        ssize_t writen_bytes;
        writen_bytes = write(fd, &new, sizeof(PROCESS_STRUCT)); 
        close(fd);

        unlink(buffername);

        if (writen_bytes > 0) {
            sprintf(buffername, "Tarefa %d - Submetida!\n", new.pid);
            write(1,buffername,MAX_BUFFER_SIZE);
        }
    }
    else if  (!strcmp(option,"status")) {
        if (argc < 2) {
            fprintf(stderr, "Uso: %s exit\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        open_fifo(&fd,MAIN_FIFO_SERVER,O_WRONLY);

        PROCESS_STRUCT new;
        new.pid = getpid();
        new.time = 0;
        new.request = STATUS;

        char *buffername = create_fifo(new.pid);
    
        ssize_t writen_bytes;
        writen_bytes = write(fd, &new, sizeof(PROCESS_STRUCT)); 
        close(fd);

        unlink(buffername);

        if (writen_bytes > 0) {
            sprintf(buffername, "Tarefa %d - Submetida!\n", new.pid);
            write(1,buffername,MAX_BUFFER_SIZE);
        }
    }
    
    return 0;
}

