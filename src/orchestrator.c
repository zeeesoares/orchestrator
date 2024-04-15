#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include "controller.h"



#define MAX_BUFFER_SIZE 300

typedef struct process_struct {
    int pid;
    int time;
    char command[MAX_BUFFER_SIZE - sizeof(int) * 2]; // comando com argumentos
} ProcessStruct;


int main(int argc, char *argv[]) {
    /*
    if (argc < 4) {
        fprintf(stderr, "Uso: %s output_folder parallel-tasks sched-policy\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    */

   int program_counter = 0;

    // Criar o pipe com nome para comunicação com o cliente
    if (mkfifo("pipe_servidor", 0666) == -1) {
        if (errno != EEXIST) {
            perror("Erro ao criar o pipe nomeado");
            exit(EXIT_FAILURE);
        }
    }

    int fd;

    // Abrir o pipe
    fd = open("pipe_servidor", O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o pipe nomeado");
        exit(EXIT_FAILURE);
    }

    ProcessStruct new;
    while (1) {
        // Ler a mensagem do cliente
        ssize_t bytes_lidos = read(fd, &new, sizeof(ProcessStruct));
        if (bytes_lidos > 0) {
            program_counter++;
            printf("Mensagem recebida do cliente: %d %d %s\n", new.pid, new.time, new.command);
            controller(1,new.command);
        }
    }

    // Fechar o pipe
    close(fd);
    unlink("pipe_servidor");  // Remover o pipe nomeado

    return 0;
} 