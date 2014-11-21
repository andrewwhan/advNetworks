#include "agent.h"
#include "hostCommands.h"

int main(){
	int socket;
	if((socket = controllerConnect()) != -1){		// establish connection with controller
		waitForCommands(socket);					// wait for commands from controller
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
	status = getaddrinfo(url, port, &hints, &res);							// get address info of host in res
	if(status != 0){
		printf("getaddrinfo error: %s\n", gai_strerror(status));
		return -1;
	}
	sockinfo = socket(res->ai_family, res->ai_socktype, res->ai_protocol);	// establish host socket to communicate
	if(sockinfo == -1){
		printf("Socket error \n");
		return -1;
	}
	if(connect(sockinfo, res->ai_addr, res->ai_addrlen) == -1){				// connect to controller socket
		printf("Connection error \n");
		close(sockinfo);
		return -1;
	}

	FILE* dbFile;															// construct first message
	dbFile = fopen("credentials.txt", "r");

	char msg[55];
	char cid = 0x04;
	uint tid = 1;
	*msg = cid;
	*(msg + 1) = tid;

	fscanf(dbFile, "%s %s", hostName, secret);								// read secret from file
	snprintf(msg + 7, sizeof(msg)-7, "%s %s", hostName, secret);
	*(msg + 8 + strlen(hostName) + strlen(secret)) = '\0';
	if(send(sockinfo, msg, 9 + strlen(hostName) + strlen(secret), 0) == -1){	// send first message
		printf("Send error \n");
		close(sockinfo);
		return -1;
	}

	return sockinfo;
}

void waitForCommands(int socket) {
	printf("awaiting response\n");
	int returned = 1;
	while (returned > 0) {
		char* msg = malloc(1500*sizeof(char));
		returned = recv(socket, msg, 1500, 0);	// hang on waiting for response
		if (returned > 0) {
			printf("command recieved");
			receiveCommand(msg, socket);
		}
	}
	return;
}
