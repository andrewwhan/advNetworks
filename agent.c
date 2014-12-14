#include "agent.h"
#include "hostCommands.h"
#include "hostPacket.h"
#include <errno.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */

int main(){
	int ctrlSocket, packetSocket;
	createResendSocket();
	packetSocket = esablishPacketSocket();
	ctrlSocket = controllerConnect();
	if(ctrlSocket != -1 && packetSocket != -1){
		listenForAciton( ctrlSocket, packetSocket);
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

int esablishPacketSocket() {
	int sockinfo;
	sockinfo = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ALL));								// establish socket
	
	struct sockaddr_ll* resendAddr = (struct sockaddr_ll*) malloc(sizeof( struct sockaddr_ll));
	resendAddr->sll_family = AF_PACKET;
	resendAddr->sll_ifindex = if_nametoindex("beans");										// set device to listen on
	resendAddr->sll_protocol = htons(ETH_P_ALL);

	if(bind(sockinfo, (struct sockaddr*) resendAddr, sizeof(struct sockaddr_ll)) == -1){	// bind socket to device
		printf("Bind error %s\n", strerror(errno));
		close(sockinfo);
		free(resendAddr);
		return -1;
	} else {
		printf("working\n");
		free(resendAddr);
		return sockinfo;
	}
}

void listenForAciton( int ctrlSocket, int packetSocket) {
	fd_set inputs;
	int numfds;
	FD_ZERO(&inputs);
	FD_SET(ctrlSocket, &inputs); //stdin
	FD_SET(packetSocket, &inputs); //listen socket
	if( ctrlSocket > packetSocket) numfds = ctrlSocket;
	else numfds = packetSocket;
	int returned;
	
	while(1) {
		select(numfds+1, &inputs, NULL, NULL, NULL);
		if( FD_ISSET( ctrlSocket, &inputs)){
			char* msg = malloc(1500*sizeof(char));
			returned = recv(ctrlSocket, msg, 1500, 0);
			if (returned > 0) {
				printf("command recieved");
				receiveCommand(msg, ctrlSocket);
			}
		}
		if( FD_ISSET( packetSocket, &inputs)){
			char* msg = malloc(1500*sizeof(char));
			returned = recv(packetSocket, msg, 1500, 0);
			if (returned > 0) {
				printf("\npacket recieved\n");
				receivePacket( msg, returned, ctrlSocket);
			}
			free(msg);
		}
	}
	return;
}
