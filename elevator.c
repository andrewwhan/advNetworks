#include "controller.h"
#include "elevator.h"

void elevate(int sockinfo){
	extern struct elevConfig* elevData;
	char* hostName = getNameBySocket(sockinfo);
	char* msg = malloc(1500*sizeof(char));
	int returned = recv(sockinfo, msg, 1500, 0);
	if(returned > 0){
		char cid = *msg;
		uint tid = *(uint*)(msg + 1);
		short dataLength = *(short*)(msg + 5);
		char* dataStart = msg + 7;

		printf("elevate:		%02X\n", cid);
		printf("tid:		%u\n", tid);
		printf("dataLength:	%hd\n", dataLength);
		fwrite(dataStart, dataLength, 1, stdout);
		printf("\n");
		char sourceAddr[INET6_ADDRSTRLEN];
		char destAddr[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, (dataStart+8), sourceAddr, INET6_ADDRSTRLEN);
		inet_ntop(AF_INET6, (dataStart+24), destAddr, INET6_ADDRSTRLEN);

		struct elevConfig* currentRule = elevData;
		printf("%s \n", currentRule->hostName);
		while(currentRule != NULL){
			if(!fnmatch(hostName, currentRule->hostName) &&
				!fnmatch(sourceAddr, currentRule->sourceAddr) &&
				!fnmatch(destAddr, currentRule->destAddr)){
				printf("EXECUTE \n");
				int i = 0;
				while(currentRule->action[i] != NULL){
					char* runAction = malloc(512 * sizeof(char));
					strcpy(runAction, currentRule->action[i]);
					parseCommandLine(runAction);
					free(runAction);
					i++;
				}
				printf("Resend this \n");
				resend(sockinfo, msg);
				break;
			}
			currentRule = currentRule->next;
		}
		if(currentRule == NULL){
			printf("Drop this \n");
			drop(sockinfo, msg);
		}
	}
	free(msg);
}

void resend(int sockinfo, char* msg){
	uint tid = *(uint*)(msg + 1);
	short dataLength = 32;
	char* dataStart = msg + 7;

	char* reply = malloc(40*sizeof(char));

	int msgLoc = 0;

	*(reply + msgLoc) = 0x81;
	msgLoc++;
	memcpy(reply + msgLoc, &tid, sizeof(uint));
	msgLoc += 4;
	memcpy(reply + msgLoc, &dataLength, sizeof(short));
	msgLoc += 2;
	memcpy(reply + msgLoc, dataStart+8, 32);
	if(send(sockinfo, reply, 39, 0) == -1){
		printf("Send error \n");
		close(sockinfo);
	} else {
		printf("Successfully sent reply: \n %u, %hi \n", *(uint*)(reply + 1), *(short*)(reply + 5));
		awaitResponse(sockinfo);
	}
	free(reply);
}

void drop(int sockinfo, char* msg){
	uint tid = *(uint*)(msg + 1);
	short dataLength = 32;
	char* dataStart = msg + 7;

	char* reply = malloc(40*sizeof(char));

	int msgLoc = 0;

	*(reply + msgLoc) = 0x82;
	msgLoc++;
	memcpy(reply + msgLoc, &tid, sizeof(uint));
	msgLoc += 4;
	memcpy(reply + msgLoc, &dataLength, sizeof(short));
	msgLoc += 2;
	memcpy(reply + msgLoc, dataStart+8, 32);
	if(send(sockinfo, reply, 39, 0) == -1){
		printf("Send error \n");
		close(sockinfo);
	} else {
		printf("Successfully sent drop\n");
		awaitResponse(sockinfo);
	}
	free(reply);
}