#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#define MAX_COMMANDS 10


int exec_command(char* arg) {
    char *exec_args[MAX_COMMANDS + 1]; // +1 for NULL terminator
    int exec_ret = 0;
    int i = 0;

    char* command = strdup(arg);
    char* token;
    char* saveptr;

    token = strsep(&command, " ");
    while (token != NULL && i < MAX_COMMANDS) {
        exec_args[i] = token;
        i++;
        token = strsep(&command, " ");
    }
    exec_args[i] = NULL;

    exec_ret = execvp(exec_args[0], exec_args);
    return exec_ret;
}
