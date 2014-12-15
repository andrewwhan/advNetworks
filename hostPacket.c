#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
//#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <net/if.h>
#include "hostPacket.h"
#include "sendAndExecute.h"
#include "list.h"
#include <errno.h>
#include <linux/if_packet.h>

#define HEADER 7

uint nextTid = 1350;
packetEntry* list = NULL;
int resendSocket;

void createResendSocket() {
	//list = addPacket(NULL, "start", 6, 0);
	resendSocket = socket(AF_INET6, SOCK_RAW, IPPROTO_RAW);
	return;
}

void receivePacket(char* msg, int returned, int ctrSock) {
	printf("call receivePacket \n");
	short dataLength = 1207;
	uint tid = nextTid;
	char cid = 0x80;
	
	if(returned>1200){
		dataLength = 1207;
	}
	else {
		dataLength = returned+7;
	}
	
	int msgLoc = 0;
	char* elevateMsg = malloc(sizeof(char) * (HEADER + dataLength));
	memcpy(elevateMsg + msgLoc, &cid, 1);
	msgLoc++;
	memcpy(elevateMsg + msgLoc, &tid, sizeof(uint));
	msgLoc += 4;
	memcpy(elevateMsg + msgLoc, &dataLength, sizeof(short));
	msgLoc += 2;
	int printable = dataLength;
	memcpy(elevateMsg + msgLoc, msg, printable-7);
	
	if(send(ctrSock, elevateMsg, dataLength, 0) == -1){
		printf("Send error \n");
	} else {
		printf("ship it \n");
		list = addPacket( list, msg, returned, tid);
		printPackets(list);
		printf("\n");
	}

	nextTid++;
	printf("elevated \n");
	free(elevateMsg);
	return;
}

int resendElevatedPacket( int tid, char* args){
	packetEntry* resendPacket = getPacket(list, tid);
	if( resendPacket != NULL) {
		printf("resend socket %d \n", resendSocket);
		struct sockaddr_in6* dest = malloc(sizeof(struct sockaddr_in6));		// destination address
		memset(dest, 0, sizeof(struct sockaddr_in6));
		dest->sin6_family = AF_INET6;
		struct in6_addr* dur = calloc(1, sizeof(struct in6_addr));
		memcpy(&(dur->s6_addr), (args + 16), 16);
		dest->sin6_addr = *dur;
		//memcpy(&(dest->sin6_addr), (args + 16), 16);
		if(sendto(resendSocket, resendPacket->packet, resendPacket->length,
			0, (struct sockaddr*) dest, sizeof(struct sockaddr_in6)) == -1) {
			printf("resend error %s\n", strerror(errno));
			return 1;
		}
		list = remPacket( list, tid);
		free(dest);
		return 0;
	}
	return 1;
}

int dropElevatedPacket( int tid){
	list = remPacket( list, tid);
	return 0;
}
