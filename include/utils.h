#ifndef UTILS_H
#define UTILS_H

#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>

#define MAIN_FIFO_SERVER "tmp/FIFO_SERVER"

void make_fifo(char *fifo_name);

char *create_fifo(pid_t pid);

void open_fifo(int *fd, char *fifo_name, int flags);

#endif