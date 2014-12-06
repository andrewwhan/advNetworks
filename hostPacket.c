#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "hostPacket.h"
#include "sendAndExecute.h"

#define HEADER 7

uint nextTid = 1350;

void receivePacket(char* msg, int returned, int ctrSock) {
	fwrite(msg, returned, 1, stdout);


	short dataLength = 1207;
	uint tid = nextTid;
	char cid = 0x80;
	
	if(returned>1200){
		dataLength = 1207;
	}
	else {
		dataLength = returned+7;
	}
	
	printf("\ndataLength:	%hd\n", dataLength);
	
	int msgLoc = 0;
	char* elevateMsg = malloc(sizeof(char) * (HEADER + dataLength));
	memcpy(elevateMsg + msgLoc, &cid, 1);
	msgLoc++;
	memcpy(elevateMsg + msgLoc, &tid, sizeof(uint));
	msgLoc += 4;
	memcpy(elevateMsg + msgLoc, &dataLength, sizeof(short));
	msgLoc += 2;
	int printable = dataLength;
	printf("sending message %d\n", printable);
	memcpy(elevateMsg + msgLoc, msg, printable-7);
	fwrite(elevateMsg, printable, 1, stdout);

	sendMessageHost(ctrSock, elevateMsg, dataLength);

	nextTid++;
	return;
}
