#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#define MAX_COMMANDS 10


int exec_command(int pid, char* arg) {
    int status;

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

    pid_t pid_fork = fork();

    if (pid_fork == 0) {
        char filename[256];
        snprintf(filename, sizeof(filename), "tmp/task_output_%d.txt", pid);
        
        int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);

        exec_ret = execvp(exec_args[0], exec_args);
        _exit(0);
    }
    wait(NULL);

    if (WEXITSTATUS(status)) {
        return EXIT_SUCCESS;
    }
    
    return EXIT_FAILURE;
}
