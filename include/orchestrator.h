#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>


#include "utils.h"
#include "client.h"
#include "controller.h"


typedef enum SCHED_POL {
    FCFS,
    SJF
} SCHED_POL;

#endif 