#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
}

void aliasCommand( char** cmdArgs) {
	printf("hi a\n");
}
void responseCommand( char** cmdArgs) {
	printf("hi r\n");
	sendMessage(0x00, cmdArgs);
}
void natCommand( char** cmdArgs) {
	printf("hi n\n");
}
