# Orchestrator :shipit:

This project develops a task orchestration service in a computer system, allowing users to submit tasks for execution on a server. The server scales and performs these tasks efficiently, keeping records for later analysis.

## Transfer and Execution
### Transferring the Project
To transfer the project to your local environment, you can clone the GitHub repository using the following command:

``` bash
git clone <REPOSITORY_URL>
```

## Compiling with Makefile
To compile the project, navigate to the project directory and use make:

``` bash
cd <PROJECT_DIRECTORY>
make
```

This will compile the project and generate the necessary executables.

## Running the Orchestrator
To run the orchestrator, use the following command:

``` bash
./bin/orchestrator <output_folder> <parallel_tasks> <sched_policy>
```

## Running the Client
To run the client and submit tasks, use the following example commands:


``` bash
./bin/client execute <time> <-u|-p> <command>
```

-u for simple linux comands | -p for pipeline commands

This command executes reading the largefile.txt file for 1000 milliseconds. The expected execution time is close to 1000 ms.

## Status and Exit
To check the program status, use the following command:


``` bash
./bin/client status
```


To exit the orchestrator:
``` bash
./bin/client exit
```
