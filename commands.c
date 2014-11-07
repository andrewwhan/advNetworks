#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "hostCommands.h"

#define HEADER 7

void sendMessage(char cid, char** cmdArgs){
	extern uint nextTid;

	int i = 0;
	short dataLength = 0;
	int msgLoc = 0;
	char space = ' ';

	while(cmdArgs[i]){ //As long as there are still arguments
		dataLength += strlen(cmdArgs[i]) + 1;
		i++;
	}
	char* messagePtr = malloc(sizeof(char) * (HEADER + dataLength));
	memcpy(messagePtr + msgLoc, &cid, 1);
	msgLoc++;
	nextTid++;
	memcpy(messagePtr + msgLoc, &nextTid, sizeof(uint));
	msgLoc += 4;
	memcpy(messagePtr + msgLoc, &dataLength, sizeof(short));
	msgLoc += 2;
	i=0;
	while(cmdArgs[i]){
		memcpy(messagePtr + msgLoc, cmdArgs[i], strlen(cmdArgs[i]));
		memcpy(messagePtr + msgLoc + strlen(cmdArgs[i]), &space, sizeof(char));
		msgLoc += strlen(cmdArgs[i]) + 1;
		i++;
	}

	for(i=0; i<msgLoc; i++){
		printf("%02X \n", messagePtr[i]);
	}
	fwrite(messagePtr, msgLoc, 1, stdout);
	
	receiveCommand();
}

void aliasCommand( char** cmdArgs) {
	int commandIndex = getCommandIndexASR( cmdArgs[0]);
	switch (commandIndex) {
		case 0:			// add 0x0
			sendMessage(0x00, cmdArgs);
			break;
		case 1:			// remove 0x1
			sendMessage(0x10, cmdArgs);
			break;
		case 2:			// show 0x2
			sendMessage(0x20, cmdArgs);
			break;
		case -1:
			printf("invalid command: add, show, remove\n");
			return;
	}
	return;
	
}
void responseCommand( char** cmdArgs) {
	int commandIndex = getCommandIndexASR( cmdArgs[0]);
	switch (commandIndex) {
		case 0:			// add
			sendMessage(0x02, cmdArgs);
			break;
		case 1:			// remove
			sendMessage(0x12, cmdArgs);
			break;
		case 2:			// show
			sendMessage(0x22, cmdArgs);
			break;
		case -1:
			printf("invalid command: add, show, remove\n");
			return;
	}
	return;
}
void natCommand( char** cmdArgs) {
	int commandIndex = getCommandIndexASR( cmdArgs[0]);
	switch (commandIndex) {
		case 0:			// add
			sendMessage(0x03, cmdArgs);
			break;
		case 1:			// remove
			sendMessage(0x13, cmdArgs);
			break;
		case 2:			// show
			sendMessage(0x23, cmdArgs);
			break;
		case -1:
			printf("invalid command: add, show, remove\n");
			return;
	}
	return;
}

int getCommandIndexASR( char* cmdName) {
	char* commandASR[] = {"add", "remove", "show"};
	int i;
	for( i = 0; i < 3; i++) {
		if( !strcmp( cmdName, commandASR[i])) {
			return i;
		}
	}
	return -1; 			// failed to identify command
}
