#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MSGLENGTH 7

void sendMessage(char cid, char** args){
	void* messagePtr = malloc(sizeof(char) * MSGLENGTH);
	memcpy(&cid, messagePtr, 1);
	return;
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
