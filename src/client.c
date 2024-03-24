#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MAX_BUFFER_SIZE 300

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Uso: %s <tempo> -u \"<programa> [args]\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *tempo = argv[1];
    char *programa = argv[3];

    // Construa a mensagem para enviar ao servidor
    char mensagem[MAX_BUFFER_SIZE];
    snprintf(mensagem, MAX_BUFFER_SIZE, "%s %s", tempo, programa);

    // Abrir o pipe nomeado para comunicação com o servidor
    int fd = open("pipe_servidor", O_WRONLY);
    if (fd == -1) {
        perror("Erro ao abrir o pipe nomeado");
        exit(EXIT_FAILURE);
    }

    // Escrever a mensagem para o servidor
    if (write(fd, mensagem, strlen(mensagem)) == -1) {
        perror("Erro ao escrever para o pipe");
        exit(EXIT_FAILURE);
    }

    // Fechar o pipe
    close(fd);

    return 0;
}