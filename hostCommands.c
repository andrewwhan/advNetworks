#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include  <sys/types.h>
#include <string.h>
#include "hostCommands.h"

void receiveCommand(char* messagePtr) {
	char cid = *messagePtr;
	uint tid = *(messagePtr + 1);
	short dataLength = *(messagePtr + 5);
	char* dataStart = messagePtr + 7;

	printf("%02X, %u \n", cid, tid);

	switch(cid){
		case 0x00:
			addIPv6Alias(cid, tid, dataLength, dataStart);
			break;
		case 0x10:
			// Remove alias command
			break;
		case 0x20:
			// Show aliases command
			break;
		case 0x32:
			// Request response from host command
			break;
		case 0x03:
			// Add nat rule command
			break;
		case 0x13:
			// Remove nat rule command
			break;
		case 0x23:
			// Show nat rules command
			break;
		case default:
			break;
	}
	return;
}

/* Command Specific Function Template
void function(char cid, uint tid, short dataLength, char* dataStart){
	
}
*/

void executeArgs(char* args[]) {

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

	return;
}
