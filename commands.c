#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define HEADER 7

void sendMessage(char cid, char** cmdArgs){
	extern uint nextTid;

	int i = 0;
	int dataLength = 0;
	while(strlen(cmdArgs[i]) != 0){
		printf("%s", cmdArgs[i]);
		dataLength += strlen(cmdArgs[i]) + 1;
		i++;
		printf("%i, %i", i, strlen(cmdArgs[i]));
	}
	void* messagePtr = malloc(sizeof(char) * HEADER);
	memcpy(&cid, messagePtr, 1);
	nextTid++;
	memcpy(&nextTid, messagePtr+1, sizeof(uint));
	short length;
}

void aliasCommand( char** cmdArgs) {
	printf("hi a\n");
}
void responseCommand( char** cmdArgs) {
	printf("hi r\n");
}
void natCommand( char** cmdArgs) {
	printf("hi n\n");
}
