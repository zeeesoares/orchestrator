#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MAX_BUFFER_SIZE 300

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Uso: %s <tempo> -u/-p \"<programa> [args]\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *tempo = argv[2];
    char *programa = argv[4];

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


    /*
    char resposta[MAX_BUFFER_SIZE];
    int bytes_lidos = read(fd, resposta, MAX_BUFFER_SIZE);
    if (bytes_lidos == -1) {
        perror("Erro ao ler do pipe");
        exit(EXIT_FAILURE);
    }
    */

    // Fechar o pipe
    close(fd);

    printf("Tarefa submetida com sucesso.\n");

    return 0;
}