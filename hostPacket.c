#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include "hostPacket.h"
#include "sendAndExecute.h"
#include "list.h"
#include <errno.h>

#define HEADER 7

uint nextTid = 1350;
packetEntry* list = NULL;
int resendSocket;

void createResendSocket() {
	//list = addPacket(NULL, "start", 6, 0);
	resendSocket = socket(AF_INET6, SOCK_RAW, IPPROTO_IPV6);
	printf("resend socket %d \n", resendSocket);
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
	}

	nextTid++;
	printf("elevated \n");
	free(elevateMsg);
	return;
}

int resendElevatedPacket( int tid, char** args){
	packetEntry* resendPacket = getPacket(list, tid);
	if( resendPacket != NULL) {
		struct sockaddr_ll* dest = malloc(sizeof(struct sockaddr_ll));		// destination address
		dest->sll_family = AF_PACKET;
		dest->sll_protocol = htons(ETH_P_ALL);
		dest->sll_ifindex = if_nametoindex("eth0");
		if( sendto(resendSocket, resendPacket->packet, resendPacket->length, 0, (struct sockaddr*) &dest, sizeof(dest)) == -1) {
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
