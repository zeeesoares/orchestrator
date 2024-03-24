#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#define MAX_BUFFER_SIZE 300

int main() {
    // Criar o pipe nomeado para comunicação com o cliente
    if (mkfifo("pipe_servidor", 0666) == -1) {
        if (errno != EEXIST) {
            perror("Erro ao criar o pipe nomeado");
            exit(EXIT_FAILURE);
        }
    }

    int fd;
    char buffer[MAX_BUFFER_SIZE];

    // Abrir o pipe nomeado para leitura
    fd = open("pipe_servidor", O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o pipe nomeado");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Ler a mensagem do cliente
        ssize_t bytes_lidos = read(fd, buffer, sizeof(buffer));
        if (bytes_lidos == -1) {
            perror("Erro ao ler do pipe");
            exit(EXIT_FAILURE);
        } else if (bytes_lidos == 0) {
            printf("O cliente fechou a conexão\n");
            break;
        } else {
            // Processar a mensagem
            buffer[bytes_lidos] = '\0';  // Adicionar terminador de string
            printf("Mensagem recebida do cliente: %s\n", buffer);
            // Implemente o processamento da mensagem aqui
        }
    }

    // Fechar o pipe
    close(fd);
    unlink("pipe_servidor");  // Remover o pipe nomeado

    return 0;
}