#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "sendAndExecute.h"

#define HEADER 7

int executeArgs(char* args[]) {

	FILE* seFile;
	seFile = fopen("hostSTDERR.txt", "w");
	pid_t  pid;
	int    status;

	if ((pid = fork()) < 0) {					// fork a child process
		printf("*** ERROR: forking child process failed\n");
		exit(1);
	} else if (pid == 0) {						// for the child process:
		dup2(fileno(seFile), STDERR_FILENO);
		fclose(seFile);
		if (execvp(args[0], args) < 0) {		// execute the command
			printf("*** ERROR: exec failed\n");
			exit(1);
		}
		exit(0);
	} else {									// for the parent:
		while (wait(&status) != pid);			// wait for completion
	}
	return !status; // convert to true/false value
}

int executeShow(char* args[]) {
	printf("show\n");
	FILE* soFile;
	soFile = fopen("hostSTDOUT.txt", "w");			// open file to pipe STDOUT to
	int status;
	pid_t   childpid;

	if((childpid = fork()) == -1) {					// fork child process
		perror("fork");
		exit(1);
	}
	if(childpid == 0) {								// execute in child
			dup2(fileno(soFile), STDOUT_FILENO);	// make STDOUT of child write to soFile
			fclose(soFile);
			if (execvp(args[0], args) < 0) {
				printf("*** ERROR: exec failed\n");
				exit(1);
			}
			exit(0);
	} else {										// execute in parent
		while (wait(&status) != childpid);			// wait for completion
	}
	return !status; // convert to true/false value
}

char* constructHeader( char cid, uint tid, short dataLength) {
	int msgLoc = 0;
	char* messagePtr = malloc(sizeof(char) * (HEADER + dataLength));
	memcpy(messagePtr + msgLoc, &cid, 1);
	msgLoc++;
	memcpy(messagePtr + msgLoc, &tid, sizeof(uint));
	msgLoc += 4;
	memcpy(messagePtr + msgLoc, &dataLength, sizeof(short));
	msgLoc += 2;
	return messagePtr;
}

void sendMessageHost(int socket, char* messagePtr, short dataLength) {
	if(send(socket, messagePtr, dataLength, 0) == -1){
		printf("Send error \n");
		close(socket);
	}
	return;
}

void fillMessageData (int argCount, char output[50][128], char* messagePtr) {
	int j = 0;
	int msgLoc = 7;
	char space = ' ';
	while( j < argCount){
		printf("%s", output[j]);
		memcpy(messagePtr + msgLoc, output[j], strlen(output[j]));
		msgLoc += strlen(output[j]) + 1;
		j++;
	}
	printf("\n");
	return;
}

void sendSuccess(char cid, uint tid, int socket) {
	printf("success\n");

	short dataLength = 0;
	int msgLoc = 0;
	char space = ' ';
	char* successMessage = "Success! specified command was executed\0";	// create data

	dataLength += strlen(successMessage) + 1;
	char* messagePtr = constructHeader(cid, tid, dataLength);			// construct message
	msgLoc = 7;

	memcpy(messagePtr + msgLoc, successMessage, strlen(successMessage));
	memcpy(messagePtr + msgLoc + strlen(successMessage), &space, sizeof(char));
	msgLoc += strlen(successMessage) + 1;

	sendMessageHost(socket, messagePtr, 7+dataLength);					// message send
	free(messagePtr);
	return;
}

void sendShow(char cid, uint tid, int socket) {
	printf("show success\n");

	FILE* soFile;
	soFile = fopen("hostSTDOUT.txt", "r");
	short dataLength = 0;
	char space = ' ';

	char output[50][128];
	int argCount = 0;

	while(fgets(output[argCount], 128, soFile) != NULL) {		// get data from output file
		dataLength += (short) strlen(output[argCount]) + 1;
		argCount++;
	}
	fclose(soFile);

	char* messagePtr = constructHeader(cid, tid, dataLength);	// create the message
	fillMessageData ( argCount, output, messagePtr);
	*(messagePtr + 7 + dataLength) = '\0';

	sendMessageHost(socket, messagePtr, 7+dataLength);			// send message
	free(messagePtr);
	return;
}

void sendFailure(char cid, uint tid, int socket) {
	printf("failure\n");
	FILE* seFile;
	seFile = fopen("hostSTDERR.txt", "r");
	short dataLength = 0;
	int msgLoc = 0;
	char space = ' ';

	char output[50][128];
	int argCount = 0;

	while(fgets(output[argCount], 128, seFile) != NULL) {		// get data from output file
		dataLength += strlen(output[argCount]) + 1;
		argCount++;
	}
	fclose(seFile);
	
	char* messagePtr = constructHeader(cid, tid, dataLength);	// create the message
	fillMessageData( argCount, output, messagePtr);
	*(messagePtr + 7 + dataLength) = '\0';

	sendMessageHost(socket, messagePtr, 7+dataLength);			// send message
	//free(messagePtr);
	return;
}
