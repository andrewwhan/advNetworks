#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MSGLENGTH 7

void sendMessage(char cid){
	void* messagePtr = malloc(sizeof(char) * MSGLENGTH);
	memcpy(&cid, messagePtr, 1);
}

void requestResponseFromHost(char** cmdArgs){
	printf("hi");
}