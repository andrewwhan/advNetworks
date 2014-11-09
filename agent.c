#include "agent.h"

int main(){
	//controllerConnect();
	return;
}

void controllerConnect(){
	const char* url = "controller.team3.4516.cs.wpi.edu";
	const char* port = "3875";
	char hostName[32];
	char secret[16];

	int status, sockinfo;
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	status = getaddrinfo(url, port, &hints, &res);
	if(status != 0){
		printf("getaddrinfo error: %s\n", gai_strerror(status));
		return;
	}
	sockinfo = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(sockinfo == -1){
		printf("Socket error \n");
		return;
	}

	if(connect(sockinfo, res->ai_addr, res->ai_addrlen) == -1){
		printf("Connection error \n");
		close(sockinfo);
		return;
	}

	FILE* dbFile;
	dbFile = fopen("credentials.txt", "r");

	char msg[48];

	fscanf(dbFile, "%s %s", hostName, secret);
	snprintf(msg, sizeof(msg), "%s %s", hostName, secret);
	if(send(sockinfo, msg, strlen(msg), 0) == -1){
		printf("Send error \n");
		close(sockinfo);
		return;
	}

	void* receive = malloc(1500*sizeof(char));
	int returned = recv(sockinfo, receive, 1500, 0);
	while(returned != 0){
		fwrite(receive, 1, returned, stdout);
		returned = recv(sockinfo, receive, 1500, 0);
	}
	free(receive);
	close(sockinfo);
	return;
}