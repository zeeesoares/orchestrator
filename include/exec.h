#ifndef EXEC_H
#define EXEC_H

#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>

#include "utils.h"

int exec_command(char* arg);

#endif 