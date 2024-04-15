#include <stdio.h>
#include "mysystem.h"

void controller(int N, char command[]) {
	pid_t pid[N];
	int i;
	for (i = 0; i < N; i++) {
		pid[i] = fork();
		if (pid[i] == 0) {
			int count;
			for (count = 0; mysystem(command) != 0; count++);
			_exit(count);
		}
	}
	for (i = 0; i < N; i++) {
		int status;
		waitpid(pid[i],&status,0);
		if (WEXITSTATUS(status))
			printf("%s %d\n",command, WEXITSTATUS(status));
	}
}




// int main(int argc, char* argv[]) {

//     char *commands[argc-1];
//     int N = 0;
// 	for(int i=1; i < argc; i++){
// 		commands[N] = strdup(argv[i]);
// 		printf("command[%d] = %s\n", N, commands[N]);
//         N++;
// 	}

//     controller(N, commands);

// 	return 0;
// }