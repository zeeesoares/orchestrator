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
        fprintf(stderr, "Uso: %s execute -time -u/-p \"<programa> [args]\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    open_fifo(&fd,MAIN_FIFO_SERVER,O_WRONLY);

    PROCESS_STRUCT new;
    new.pid = getpid();
    new.time = atoi(argv[2]);
    printf("%d\n",new.time);
    printf("%d\n",new.pid);
    strcpy(new.command, argv[4]);

    char *buffername = create_fifo(new.pid);
    
    write(fd, &new, sizeof(PROCESS_STRUCT)); 
    close(fd);

    unlink(buffername);

    return 0;
}
