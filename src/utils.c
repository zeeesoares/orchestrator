#include "utils.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void make_fifo(char *fifo_name) {
  if (mkfifo(fifo_name, 0666) == -1) {
    perror("mkfifo");
    exit(EXIT_FAILURE);
  }
}

char *create_fifo(pid_t pid) {
  char *fifo_name = malloc(sizeof(char) * 64);
  sprintf(fifo_name, "tmp/FIFO_%d", pid);  // NOLINT

  if (mkfifo(fifo_name, 0666) == -1) {
    perror("mkfifo");
    exit(EXIT_FAILURE);
  }

  return fifo_name;
}

void open_fifo(int *fd, char *fifo_name, int flags) {
  *fd = open(fifo_name, flags);

  if (*fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }
}

const char* getRequestType(REQUEST_TYPE req) {
   switch (req) 
   {
      case NEW: return "New";
      case COMMAND: return "Command";
      case STATUS: return "Status";
      case PIPELINE: return "Pipeline";
      case EXIT: return "Exit";
      default:  return "Null";
   }
}