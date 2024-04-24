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

    if (argc < 5) {
        fprintf(stderr, "Uso: %s <tempo> -u/-p \"<programa> [args]\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    

    fd = open(MAIN_FIFO_SERVER, O_WRONLY);
    if (fd == -1) {
        perror("Erro ao abrir o FIFO para escrita");
        exit(EXIT_FAILURE);
    }

    PROCESS_STRUCT new;
    new.pid = getpid();
    new.time = atoi(argv[3]);
    strcpy(new.command, argv[4]);

    char *buffername = create_fifo(new.pid);
    
    write(fd, &new, sizeof(PROCESS_STRUCT)); 
    close(fd);

    unlink(buffername);

    return 0;
}
