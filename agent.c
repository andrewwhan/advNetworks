#include "agent.h"

int main(){
	int socket;
	if((socket = controllerConnect()) != -1){
		//waitForCommand(socket);
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
	hints.ai_family = AF_INET;
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
	struct sockaddr_in* theiraddress = (struct sockaddr_in*)res->ai_addr;
	int i = 0;
	for(i = 0; i < 4; i++){
	printf("%hhd \n", *((char*)&(theiraddress->sin_addr.s_addr) + i));
	}
	if(connect(sockinfo, res->ai_addr, res->ai_addrlen) == -1){
		printf("Connection error \n");
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
	if(send(sockinfo, msg, 8 + strlen(hostName) + strlen(secret), 0) == -1){
		printf("Send error \n");
		close(sockinfo);
		return -1;
	}

	return sockinfo;
}
