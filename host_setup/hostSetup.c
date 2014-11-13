#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

void main() {
	pid_t  pid;
	int    status;
	
	char* args[] = {"ssh", "-t", "cs4516@host1.team3.4516.cs.wpi.edu", "\"sudo /root/shared/setupScripts/setupHost1\"", '\0'};
	int i;
	//for(i = 0; i < 4; i++){
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
	//}

	printf("done setting up host1\n");
	return; // convert to true/false value
}
