#include "client.h"
#include "utils.h"

void send_execute_request(int fd, char* argv[]) {
    
    Task new;
    new.pid = getpid();
    new.time = atoi(argv[2]);
    new.type = EXECUTE;
    strcpy(new.command, argv[4]);

    char *buffername = create_fifo(new.pid);
    
    ssize_t writen_bytes;
    writen_bytes = write(fd, &new, sizeof(Task)); 
    close(fd);

    int fd_ret;
    int id;
    open_fifo(&fd,buffername,O_RDONLY);

    size_t bytes_read;
    bytes_read = read(fd_ret, &id, sizeof(int));
    if (bytes_read == -1) {
        perror("Erro ao ler do FIFO de retorno");
        exit(1);
    }

    close(fd_ret);
    unlink(buffername);

    write(1,id,sizeof(int));
}

void send_status_request(int fd) {
    // code to send STATUS request to server
}

void send_exit_request(int fd) {
    // code to send EXIT request to server
}

int main(int argc, char *argv[]) {
    int fd;

    open_fifo(&fd,MAIN_FIFO_SERVER,O_WRONLY);

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <option>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    REQUEST_TYPE option = getRequestType(argv[1]);

    switch (option)
    {
    case EXECUTE:
        if (argc < 5) {
            fprintf(stderr, "Uso: %s execute -time -u/-p \"<programa> [args]\"\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        send_execute_request(fd,argv);
        break;
    case STATUS:
        /* code */
        break;
    case EXIT:
        /* code */
        break;
    default:
        break;
    }

    return 0;
}

