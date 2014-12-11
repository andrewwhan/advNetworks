#include "agent.h"
#include "hostCommands.h"
#include "hostPacket.h"
#include <errno.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */

int main(){
	int socket = 5;
	waitForPackets(socket);
	if((socket = controllerConnect()) != -1){
		waitForCommands(socket);
	}
	return;
}

int controllerConnect(){

	const char* url = "controller.team3.4516.cs.wpi.edu";
	const char* port = "3875";
	char hostName[32];
	char secret[16];

	int status, sockinfo;
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	status = getaddrinfo(url, port, &hints, &res);
	if(status != 0){
		printf("getaddrinfo error: %s\n", gai_strerror(status));
		return -1;
	}
	sockinfo = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(sockinfo == -1){
		printf("Socket error \n");
		return -1;
	}
	if(connect(sockinfo, res->ai_addr, res->ai_addrlen) == -1){
		printf("Connection error %s\n", strerror(errno));
		close(sockinfo);
		return -1;
	}

	FILE* dbFile;
	dbFile = fopen("credentials.txt", "r");

	char msg[55];
	char cid = 0x04;
	uint tid = 1;
	*msg = cid;
	*(msg + 1) = tid;

	fscanf(dbFile, "%s %s", hostName, secret);
	snprintf(msg + 7, sizeof(msg)-7, "%s %s", hostName, secret);
	*(msg + 8 + strlen(hostName) + strlen(secret)) = '\0';
	if(send(sockinfo, msg, 9 + strlen(hostName) + strlen(secret), 0) == -1){
		printf("Send error \n");
		close(sockinfo);
		return -1;
	}

	return sockinfo;
}

void waitForPackets( int ctrSock) {
	printf("hello\n");

	int sockinfo, returned;
	
	sockinfo = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));							// establish socket
	
	struct sockaddr_ll* bindr = (struct sockaddr_ll*) malloc(sizeof( struct sockaddr_ll));
	bindr->sll_ifindex = if_nametoindex("beans");										// set device to listen on
	bindr->sll_protocol = htons(ETH_P_ALL);
	bindr->sll_family = AF_PACKET;

	if(bind(sockinfo, (struct sockaddr*) bindr, sizeof(struct sockaddr_ll)) == -1){		// bind socket to device
		printf("Bind error %s\n", strerror(errno));
		close(sockinfo);
		return;
	} else {
		printf("working\n");
	}
	
	createResendSocket();
	
	char* msg = malloc(1500*sizeof(char));
	returned = recv(sockinfo, msg, 1500, 0);

	while( returned > 0){										// wait on packets to be received
		if (returned > 0) {
			printf("\npacket recieved\n");
			receivePacket( msg, returned, ctrSock);
		}
		returned = recv(sockinfo, msg, 1500, 0);
	}
	free(msg);
	free(bindr);
	printf("weMadeIt\n");
}


void waitForCommands(int socket) {
	int returned = 1;
	while (returned > 0) {
		char* msg = malloc(1500*sizeof(char));
		returned = recv(socket, msg, 1500, 0);
		if (returned > 0) {
			printf("command recieved");
			receiveCommand(msg, socket);
		}
	}
	return;
}
