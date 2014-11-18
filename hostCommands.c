#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "hostCommands.h"

#define HEADER 7

void receiveCommand(char* messagePtr, int socket) {
	char cid = *messagePtr;
	uint tid = *(uint*)(messagePtr + 1);
	short dataLength = *(short*)(messagePtr + 5);
	char* dataStart = messagePtr + 7;

	printf("cid:		%02X\n", cid);
	printf("tid:		%u\n", tid);
	printf("dataLength:	%hd\n", dataLength);
	printf("dataStart:	%s\n", dataStart);
	int i;

	int status;

	switch(cid){
		case 0x00:
			// Add alias command
			status = addIPv6Alias(cid, tid, dataLength, dataStart);
			break;
		case 0x10:
			// Remove alias command
			status = removeIPv6Alias(cid, tid, dataLength, dataStart);
			break;
		case 0x20:
			// Show aliases command
			status = showIPv6Alias(cid, tid, dataLength, dataStart);
			if( status) {
				status = 3; // indicate that it is a show command that executed correctly
			}
			break;
		case 0x32:
			// Request response from host command
			status = !0; // send success response
			break;
		case 0x03:
			// Add nat rule command
			status = addNatRule(cid, tid, dataLength, dataStart);
			break;
		case 0x13:
			// Remove nat rule command
			status = removeNatRule(cid, tid, dataLength, dataStart);
			break;
		case 0x23:
			// Show nat rules command
			status = showNatRule(cid, tid, dataLength, dataStart);
			if( status) {
				status = 3; // indicate that it is a show command that executed correctly
			}
			break;
		default:
			break;
	}
	if(status == 3) {
		sendShow(cid, tid, socket);
	} else if (status) {
		sendSuccess(cid, tid, socket);
	} else {
		sendFailure(cid, tid, socket);
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

	if ((pid = fork()) < 0) {					/* fork a child process		*/
		printf("*** ERROR: forking child process failed\n");
		exit(1);
	} else if (pid == 0) {						/* for the child process:	*/
		dup2(fileno(seFile), STDERR_FILENO);
		fclose(seFile);
		if (execvp(args[0], args) < 0) {		/* execute the command		*/
			printf("*** ERROR: exec failed\n");
			exit(1);
		}
		exit(0);
	} else {									/* for the parent:		*/
		while (wait(&status) != pid);			/* wait for completion		*/
	}
	return !status; // convert to true/false value
}

int executeShow(char* args[]) {
	printf("show\n");
	FILE* soFile;
	soFile = fopen("hostSTDOUT.txt", "w");
	int status;
	pid_t   childpid;

	if((childpid = fork()) == -1) {
		perror("fork");
		exit(1);
	}
	if(childpid == 0) {
			dup2(fileno(soFile), STDOUT_FILENO);
			fclose(soFile);
			if (execvp(args[0], args) < 0) {
				printf("*** ERROR: exec failed\n");
				exit(1);
			}
			exit(0);
	} else {
		while (wait(&status) != childpid);			/* wait for completion		*/
	}
	return !status; // convert to true/false value
}

void sendSuccess(char cid, uint tid, int socket) {
	printf("success\n");

	short dataLength = 0;
	int msgLoc = 0;
	char space = ' ';
	char* successMessage = "Success! specified command was executed\0";

	dataLength += strlen(successMessage) + 1;

	char* messagePtr = malloc(sizeof(char) * (HEADER + dataLength));
	memcpy(messagePtr + msgLoc, &cid, 1);
	msgLoc++;
	memcpy(messagePtr + msgLoc, &tid, sizeof(uint));
	msgLoc += 4;
	memcpy(messagePtr + msgLoc, &dataLength, sizeof(short));
	msgLoc += 2;

	memcpy(messagePtr + msgLoc, successMessage, strlen(successMessage));
	memcpy(messagePtr + msgLoc + strlen(successMessage), &space, sizeof(char));
	msgLoc += strlen(successMessage) + 1;

	*(messagePtr + 7 + dataLength) = '\0';

	if(send(socket, messagePtr, 7+dataLength, 0) == -1){
		printf("Send error \n");
		close(socket);
		free(messagePtr);
		return;
	}
	free(messagePtr);
	return;
}

void sendShow(char cid, uint tid, int socket) {
	printf("show success\n");

	FILE* soFile;
	soFile = fopen("hostSTDOUT.txt", "r");
	short dataLength = 0;
	int msgLoc = 0;
	char space = ' ';

	char output[50][128];
	int i = 0;

	while(fgets(output[i], 128, soFile) != NULL) { //As long as there are still arguments
		dataLength += (short) strlen(output[i]) + 1;
		//printf("current dataLength: %hd of output: %s", dataLength, output[i]);
		i++;
	}
	//printf("\n");

	char* messagePtr = malloc(sizeof(char) * (HEADER + dataLength));
	memcpy(messagePtr + msgLoc, &cid, 1);
	msgLoc++;
	memcpy(messagePtr + msgLoc, &tid, sizeof(uint));
	msgLoc += 4;
	memcpy(messagePtr + msgLoc, &dataLength, sizeof(short));
	msgLoc += 2;

	int j=0;
	while( j < i){
		printf("%s", output[j]);
		memcpy(messagePtr + msgLoc, output[j], strlen(output[j]));
		memcpy(messagePtr + msgLoc + strlen(output[i]), &space, sizeof(char));
		msgLoc += strlen(output[j]) + 1;
		j++;
	}
	*(messagePtr + 7 + dataLength) = '\0';

	if(send(socket, messagePtr, 7+dataLength, 0) == -1){
		printf("Send error \n");
		close(socket);
		fclose(soFile);
		free(messagePtr);
		return;
	}
	fclose(soFile);
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
	int i = 0;

	while(fgets(output[i], 128, seFile) != NULL) { //As long as there are still arguments
		dataLength += strlen(output[i]) + 1;
		i++;
	}
	fclose(seFile);
	char* messagePtr = malloc(sizeof(char) * (HEADER + dataLength));
	memcpy(messagePtr + msgLoc, &cid, 1);
	msgLoc++;
	memcpy(messagePtr + msgLoc, &tid, sizeof(uint));
	msgLoc += 4;
	memcpy(messagePtr + msgLoc, &dataLength, sizeof(short));
	msgLoc += 2;

	int j=0;
	while( j < i){
		printf("%s", output[j]);
		memcpy(messagePtr + msgLoc, output[j], strlen(output[j]));
		memcpy(messagePtr + msgLoc + strlen(output[i]), &space, sizeof(char));
		msgLoc += strlen(output[j]) + 1;
		j++;
	}
	printf("\n");

	if(send(socket, messagePtr, 7+dataLength, 0) == -1){
		printf("Send error \n");
		close(socket);
		free(messagePtr);
		return;
	}
	free(messagePtr);
	return;
}

int addIPv6Alias(char cid, uint tid, short dataLength, char* dataStart) {
	char* cmdtok [32];
	const char* delimiter = " \n";
	int tokcnt = 1;

	cmdtok[0] = strtok( dataStart, delimiter);			// tokenize command
	while( cmdtok[tokcnt-1]){
		cmdtok[tokcnt] = strtok( NULL, delimiter);
		tokcnt++;
	}

	char* args[32] = {"ifconfig", cmdtok[4], "inet6", "add", cmdtok[3], '\0' };

	int success = executeArgs(args);

	return success;
}

int removeIPv6Alias(char cid, uint tid, short dataLength, char* dataStart) {
	char* cmdtok [32];
	const char* delimiter = " \n";
	int tokcnt = 1;
	int success;

	cmdtok[0] = strtok( dataStart, delimiter);			// tokenize command
	while( cmdtok[tokcnt-1]){
		cmdtok[tokcnt] = strtok( NULL, delimiter);
		tokcnt++;
	}
	
	if(cmdtok[4] != NULL && cmdtok[3] != NULL) {

		char* args[32] = {"ifconfig", cmdtok[4], "inet6", "del", cmdtok[3], '\0' };
		success = executeArgs(args);
	} else {
		success = 0;
	}

	return success;
}

int showIPv6Alias(char cid, uint tid, short dataLength, char* dataStart) {

	char* args[32] = { "ifconfig", '\0' };

	int success = executeShow(args);

	return success;
}

int addNatRule(char cid, uint tid, short dataLength, char* dataStart) {
	char* cmdtok [32];
	const char* delimiter = " \n";
	int tokcnt = 1;

	cmdtok[0] = strtok( dataStart, delimiter);			// tokenize command
	while( cmdtok[tokcnt-1]){
		cmdtok[tokcnt] = strtok( NULL, delimiter);
		tokcnt++;
	}

	char* args[32] = {"ip6tables", "-t", "nat", "-I", cmdtok[3], cmdtok[4], "-p", "tcp",
		cmdtok[5], cmdtok[6], cmdtok[7], cmdtok[8], cmdtok[9], cmdtok[10], '\0' };
	int i = 0;

	int success = executeArgs(args);

	return success;
}

int removeNatRule(char cid, uint tid, short dataLength, char* dataStart) {
	char* cmdtok [32];
	const char* delimiter = " \n";
	int tokcnt = 1;

	cmdtok[0] = strtok( dataStart, delimiter);			// tokenize command
	while( cmdtok[tokcnt-1]){
		cmdtok[tokcnt] = strtok( NULL, delimiter);
		tokcnt++;
	}

	char* args[32] = {"ip6tables", "-t", "nat", "-D", cmdtok[3], "-p", "tcp",
		cmdtok[4], cmdtok[5], cmdtok[6], cmdtok[7], cmdtok[8], cmdtok[9], '\0' };
	int i = 0;

	int success = executeArgs(args);

	return success;
}

int showNatRule(char cid, uint tid, short dataLength, char* dataStart) {

	char* args[32] = { "ip6tables", "-t", "nat", "-L", '\0' };

	int success = executeShow(args);

	return success;
}
