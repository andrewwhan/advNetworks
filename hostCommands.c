#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "hostCommands.h"

void receiveCommand() {
	printf("\n\n");
	char* args[] =  {"ifconfig", '\0'};
	executeArgs(args);
	return;
}

int executeArgs(char* args[]) {

	pid_t  pid;
	int    status;

	if ((pid = fork()) < 0) {				/* fork a child process		*/
		printf("*** ERROR: forking child process failed\n");
		exit(1);
	} else if (pid == 0) {					/* for the child process:	*/
		if (execvp(args[0], args) < 0) {		/* execute the command		*/
			printf("*** ERROR: exec failed\n");
			exit(1);
		}
		exit(0);
	} else {						/* for the parent:		*/
		while (wait(&status) != pid);			/* wait for completion		*/
	}
	printf("%d\n", status);
	return status;
}
