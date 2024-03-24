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

    return 0;
}