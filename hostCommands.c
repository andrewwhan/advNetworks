#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "hostCommands.h"

void receiveCommand(char* messagePtr) {
	char cid = *messagePtr;
	uint tid = *(messagePtr + 1);
	short dataLength = *(messagePtr + 5);
	char* dataStart = messagePtr + 7;

	//printf("%02X, %u \n", cid, tid);

	printf("cid:		%02X\n", cid);
	printf("tid:		%u\n", tid);
	printf("dataLength:	%hd\n", dataLength);
	printf("dataStart:	%s\n", dataStart);

	switch(cid){
		case 0x00:
			addIPv6Alias(cid, tid, dataLength, dataStart);
			break;
		case 0x10:
			removeIPv6Alias(cid, tid, dataLength, dataStart);
			// Remove alias command
			break;
		case 0x20:
			showIPv6Alias(cid, tid, dataLength, dataStart);
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
		default:
			break;
	}
	return;
}

/* Command Specific Function Template
void function(char cid, uint tid, short dataLength, char* dataStart){
	
}
*/

int executeArgs(char* args[]) {

	FILE* seFile;
	seFile = fopen("hostSTDERR.txt", "w");
	pid_t  pid;
	int    status;

	if ((pid = fork()) < 0) {				/* fork a child process		*/
		printf("*** ERROR: forking child process failed\n");
		exit(1);
	} else if (pid == 0) {					/* for the child process:	*/
		dup2(fileno(seFile), STDERR_FILENO);
		fclose(seFile);
		if (execvp(args[0], args) < 0) {		/* execute the command		*/
			printf("*** ERROR: exec failed\n");
			exit(1);
		}
		exit(0);
	} else {						/* for the parent:		*/
		while (wait(&status) != pid);			/* wait for completion		*/
	}
	return !status; // convert to true/false value
}

int executeShow(char* args[]) {
	FILE* soFile;
	soFile = fopen("hostSTDOUT.txt", "w");
	int status;
        pid_t   childpid;
        
        if((childpid = fork()) == -1)
        {
                perror("fork");
                exit(1);
        }
        if(childpid == 0)
        {
		dup2(fileno(soFile), STDOUT_FILENO);
		fclose(soFile);
		if (execvp(args[0], args) < 0) {
			printf("*** ERROR: exec failed\n");
			exit(1);
		}
                exit(0);
        }
        else
        {
		while (wait(&status) != childpid);			/* wait for completion		*/
		char textStuff[128];
		char end = '0';
		fclose(soFile);
        }
	return !status; // convert to true/false value
}

void sendSuccess(char cid, uint tid) {
	printf("success\n");
	return;
}

void sendShow(char cid, uint tid) {
	printf("show\n");
	return;
}

void sendFailure(char cid, uint tid) {
	printf("failure\n");
	return;
}

void addIPv6Alias(char cid, uint tid, short dataLength, char* dataStart) {
	char* cmdtok [32];
	const char* delimiter = " \n";
	int tokcnt = 1;

	cmdtok[0] = strtok( dataStart, delimiter);			// tokenize command
	while( cmdtok[tokcnt-1]){
		cmdtok[tokcnt] = strtok( NULL, delimiter);
		tokcnt++;
	}

	printf("cmdtok: %s, %s, %s\n", cmdtok[0], cmdtok[4], cmdtok[3]);

	char* args[32] = {"ifconfig", cmdtok[4], "inet6", "add", cmdtok[3], '\0' };

	int success = executeArgs(args);

	if(success) {
		sendSuccess(cid, tid);
	} else {
		sendFailure(cid, tid);
	}
	return;
}

void removeIPv6Alias(char cid, uint tid, short dataLength, char* dataStart) {
	char* cmdtok [32];
	const char* delimiter = " \n";
	int tokcnt = 1;

	cmdtok[0] = strtok( dataStart, delimiter);			// tokenize command
	while( cmdtok[tokcnt-1]){
		cmdtok[tokcnt] = strtok( NULL, delimiter);
		tokcnt++;
	}

	printf("cmdtok: %s, %s, %s\n", cmdtok[0], cmdtok[4], cmdtok[3]);

	char* args[32] = {"ifconfig", cmdtok[4], "inet6", "del", cmdtok[3], '\0' };

	int success = executeArgs(args);

	if(success) {
		sendSuccess(cid, tid);
	} else {
		sendFailure(cid, tid);
	}
	return;
}

void showIPv6Alias(char cid, uint tid, short dataLength, char* dataStart) {

	char* args[32] = { "ip", "addr", "show", '\0' };

	int success = executeShow(args);

	if(success) {
		sendShow(cid, tid);
	} else {
		sendFailure(cid, tid);
	}
	return;
}
