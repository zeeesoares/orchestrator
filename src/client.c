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
        new.request = NEW;
        strcpy(new.command, argv[4]);

        char *buffername = create_fifo(new.pid);
    
        ssize_t writen_bytes;
        if ((writen_bytes = write(fd, &new, sizeof(PROCESS_STRUCT))) < 0) {
            perror("ERRO AO ESCREVER!");
            exit(EXIT_FAILURE);   
        }

        close(fd);

        open_fifo(&fd,buffername,O_RDONLY);

        int task_id;
        ssize_t read_bytes = read(fd, &task_id, sizeof(int));
        if (read_bytes > 0) {
            printf("Tarefa %d - Submetida!\n", task_id);
        }

        unlink(buffername);
        close(fd);
    }
    else if  (!strcmp(option,"exit")) {
        if (argc != 2) {
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
    else if (!strcmp(option, "status")) {
        if (argc != 2) {
            fprintf(stderr, "Uso: %s status\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        open_fifo(&fd, MAIN_FIFO_SERVER, O_WRONLY);

        PROCESS_STRUCT new;
        new.pid = getpid();
        new.time = 1;
        new.request = STATUS;

        char *buffername = create_fifo(new.pid);
        char message[MAX_BUFFER_SIZE]; // Alocar memória para a mensagem

        ssize_t writen_bytes;
        writen_bytes = write(fd, &new, sizeof(PROCESS_STRUCT)); 
        close(fd);

        if (writen_bytes > 0) {
            sprintf(message, "Tarefa %d - Submetida!\n", new.pid);
            write(1, message, strlen(message)); // Use strlen para enviar apenas a parte da mensagem que foi preenchida
        }

        // Abre o FIFO do cliente para leitura
        int fd_client;
        open_fifo(&fd_client, buffername, O_RDONLY);

        // Ler a resposta do servidor e escrevê-la na saída padrão
        char buffer[MAX_RESPONSE_SIZE];
        ssize_t read_bytes;
        while ((read_bytes = read(fd_client, buffer, MAX_RESPONSE_SIZE)) > 0) {
            write(1, buffer, read_bytes);
        }

        // Fechar o FIFO do cliente
        close(fd_client);
    }

    
    return 0;
}

