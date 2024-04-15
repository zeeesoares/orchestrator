#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MAX_BUFFER_SIZE 300

typedef struct process_struct {
    int pid;
    int time;
    char command[MAX_BUFFER_SIZE - sizeof(int) * 2]; // comando com argumentos
} ProcessStruct;

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Uso: %s <tempo> -u/-p \"<programa> [args]\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    ProcessStruct message;
    message.pid = getpid();
    message.time = atoi(argv[3]);
    strcpy(message.command, argv[4]); // Copia o comando e argumentos para a estrutura de dados
    
    // Abrir o pipe nomeado para comunicação com o servidor
    int fd = open("pipe_servidor", O_WRONLY);
    if (fd == -1) {
        perror("Erro ao abrir o pipe nomeado");
        exit(EXIT_FAILURE);
    }

    // Escrever a mensagem para o servidor
    if (write(fd, &message, sizeof(ProcessStruct)) == -1) {
        perror("Erro ao escrever para o pipe");
        exit(EXIT_FAILURE);
    }

    // Fechar o pipe
    close(fd);

    printf("Tarefa submetida com sucesso.\n");

    return 0;
}
