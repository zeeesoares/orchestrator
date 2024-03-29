#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#define MAX_BUFFER_SIZE 300

int main(int argc, char *argv[]) {
    /*
    if (argc < 4) {
        fprintf(stderr, "Uso: %s output_folder parallel-tasks sched-policy\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    */

    // Criar o pipe com nome para comunicação com o cliente
    if (mkfifo("pipe_servidor", 0666) == -1) {
        if (errno != EEXIST) {
            perror("Erro ao criar o pipe nomeado");
            exit(EXIT_FAILURE);
        }
    }

    int fd;
    char buffer[MAX_BUFFER_SIZE];

    // Abrir o pipe
    fd = open("pipe_servidor", O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o pipe nomeado");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Ler a mensagem do cliente
        ssize_t bytes_lidos = read(fd, buffer, sizeof(buffer));
        if (bytes_lidos > 0) {
            buffer[bytes_lidos] = '\0';
            printf("Mensagem recebida do cliente: %s\n", buffer);
        }
    }

    // Fechar o pipe
    close(fd);
    unlink("pipe_servidor");  // Remover o pipe nomeado

    return 0;
} 