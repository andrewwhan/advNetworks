#include "commands.h"
#include "controller.h"

#define HEADER 7

void sendMessage(char cid, char** cmdArgs){
	extern uint nextTid;
	extern struct hostInfo* firstHost;

	int i = 0;
	short dataLength = 0;
	int msgLoc = 0;
	char space = ' ';

	while(cmdArgs[i]){ //As long as there are still arguments
		dataLength += strlen(cmdArgs[i]) + 1;
		i++;
	}
	char* messagePtr = malloc(sizeof(char) * (HEADER + dataLength));
	memcpy(messagePtr + msgLoc, &cid, 1);
	msgLoc++;
	nextTid++;
	memcpy(messagePtr + msgLoc, &nextTid, sizeof(uint));
	msgLoc += 4;
	memcpy(messagePtr + msgLoc, &dataLength, sizeof(short));
	msgLoc += 2;
	i=0;
	while(cmdArgs[i]){
		memcpy(messagePtr + msgLoc, cmdArgs[i], strlen(cmdArgs[i]));
		memcpy(messagePtr + msgLoc + strlen(cmdArgs[i]), &space, sizeof(char));
		msgLoc += strlen(cmdArgs[i]) + 1;
		i++;
	}
	*(messagePtr + msgLoc - 1) = '\0';
	// for(i=0; i<msgLoc; i++){
	// 	printf("%02X \n", messagePtr[i]);
	// }
	//receiveCommand(messagePtr); // instead send the message through to host
	int sockinfo = getSocketByName(cmdArgs[1]); 
	if(sockinfo < 0){
		sockinfo = getSocketByName(cmdArgs[2]);
	}
	if(sockinfo > 0){
		if(send(sockinfo, messagePtr, 7+dataLength, 0) == -1){
			printf("Send error \n");
			close(sockinfo);
		} else {
			printf("Successfully sent\n");
			awaitResponse(sockinfo);
		}
	}
	free(messagePtr);
	return;
}

void awaitResponse( int sockinfo) {
	char* msg = malloc(1500*sizeof(char));
	while(1){
		int returned = recv(sockinfo, msg, 7, 0);
		if(returned > 0 && (unsigned char)*msg != 0x80){
			char cid = *msg;
			uint tid = *(uint*)(msg + 1);
			short dataLength = *(short*)(msg + 5);
			char* dataStart = msg + 7;
			if(dataLength > 0){
				returned = recv(sockinfo, msg+7, dataLength, 0);
			}
			printf("Response cid:		%02X\n", (unsigned char)cid);
			printf("tid:		%u\n", tid);
			printf("dataLength:	%hd\n", dataLength);
			if(dataLength > 0){
				fwrite(dataStart, dataLength, 1, stdout);
				printf("\n");
			}
			free(msg);
			return;
		}
		else if(returned > 0 && (unsigned char)(*msg) == 0x80){

			char cid = *msg;
			uint tid = *(uint*)(msg + 1);
			short dataLength = *(short*)(msg + 5);
			char* dataStart = msg + 7;
			returned = recv(sockinfo, msg+7, dataLength, 0);
			printf("ignored elevation request \n");
		}
		else{
			free(msg);
			return;
		}
	}
}

void aliasCommand( char** cmdArgs) {
	int commandIndex = getCommandIndexASR( cmdArgs[1]);
	switch (commandIndex) {
		case 0:			// add 0x0, expected args count = 5
			if (countArgs(cmdArgs) == 5) sendMessage(0x00, cmdArgs);
			else printf("Format error. Try using: alias add [HOSTNAME] [IP ADDRESS] [INTERFACE]\n");
			break;
		case 1:			// remove 0x1, expected args count = 5
			if (countArgs(cmdArgs) == 5) sendMessage(0x10, cmdArgs);
			else printf("Format error. Try using: alias remove [HOSTNAME] [IP ADDRESS] [INTERFACE]\n");
			break;
		case 2:			// show 0x2, expected args count = 3
			if(countArgs(cmdArgs) == 3) sendMessage(0x20, cmdArgs);
			else printf("Format error. Try using: alias show [HOSTNAME]\n");
			break;
		case -1:
			printf("Invalid alias command. Try using: add, show, remove\n");
			return;
	}
	return;
}

void responseCommand( char** cmdArgs) {
	int commandIndex = getCommandIndexASR( cmdArgs[0]);
	switch (commandIndex) {
		case 0:			// add
			sendMessage(0x02, cmdArgs);
			break;
		case 1:			// remove
			sendMessage(0x12, cmdArgs);
			break;
		case 2:			// show
			sendMessage(0x22, cmdArgs);
			break;
		case -1:
			printf("invalid command: add, show, remove\n");
			return;
	}
	return;
}

void natCommand( char** cmdArgs) {
	int commandIndex = getCommandIndexASR( cmdArgs[1]);
	switch (commandIndex) {
		case 0:			// add, expected args count = 10
			if (countArgs(cmdArgs) == 11) sendMessage(0x03, cmdArgs);
			else {
				printf("Format error. Try using: nat add [HOSTNAME] ");
				printf("[CHAIN] [POSITION] -d [DEST IP] -j DNAT --to-destination [NEW DEST IP]\n");
			}
			break;
		case 1:			// remove, expected args count = 10
			if (countArgs(cmdArgs) == 10) sendMessage(0x13, cmdArgs);
			else {
				printf("Format error. Try using: nat remove [HOSTNAME] ");
				printf("[CHAIN] -d [DEST IP] -j DNAT --to-destination [NEW DEST IP]\n");
			}
			break;
		case 2:			// show, expected args count = 3
			if (countArgs(cmdArgs) == 3) sendMessage(0x23, cmdArgs);
			else printf("Incorrect format. Use: nat show [HOSTNAME]\n");
			break;
		case -1:
			printf("Invalid nat command. Try using: add, show, remove\n");
			return;
	}
	return;
}

void exitCommand( char** cmdArgs) {
	sendMessage(0x0A, cmdArgs);
}

void neighCommand( char** cmdArgs) {
	int commandIndex = getCommandIndexASR( cmdArgs[1]);
	switch (commandIndex) {
		case 0:			// add, expected args count = 5
			if (countArgs(cmdArgs) == 5) sendMessage(0x08, cmdArgs);
			else printf("Format error. Try using: neigh add [HOSTNAME] [IP ADDRESS] [HW ADDRESS]\n");
			break;
		case 1:			// remove, expected args count = 5
			if (countArgs(cmdArgs) == 5) sendMessage(0x18, cmdArgs);
			else printf("Format error. Try using: neigh remove [HOSTNAME] [IP ADDRESS] [HW ADDRESS]\n");
			break;
		case 2:			// show, expected args count = 3
			if(countArgs(cmdArgs) == 3) sendMessage(0x28, cmdArgs);
			else printf("Format error. Try using: neigh show [HOSTNAME]\n");
			break;
		case -1:
			printf("Invalid neigh command. Try using: add, show, remove\n");
			return;
	}
	return;
}

void routeCommand( char** cmdArgs) {
	int commandIndex = getCommandIndexASR( cmdArgs[1]);
	switch (commandIndex) {
		case 0:			// add 0x0, expected args count = 5
			if (countArgs(cmdArgs) > 3) sendMessage(0x01, cmdArgs);
			else printf("Format error. Try using: route add [HOSTNAME] [IP ADDRESS] [INTERFACE]\n");
			break;
		case 1:			// remove 0x1, expected args count = 5
			if (countArgs(cmdArgs) > 3) sendMessage(0x11, cmdArgs);
			else printf("Format error. Try using: route remove [HOSTNAME] [IP ADDRESS] [INTERFACE]\n");
			break;
		case 2:			// show 0x2, expected args count = 3
			if(countArgs(cmdArgs) == 4) sendMessage(0x21, cmdArgs);
			else printf("Format error. Try using: route show [HOSTNAME] [TABLE NAME]\n");
			break;
		case -1:
			printf("Invalid route command. Try using: add, show, remove\n");
			return;
	}
	return;
}

void fireruleCommand( char** cmdArgs) {
	int commandIndex = getCommandIndexASR( cmdArgs[1]);
	switch (commandIndex) {
		case 0:			// add 0x0, expected args count = 5
			if (countArgs(cmdArgs) > 3) sendMessage(0x05, cmdArgs);
			else printf("Format error. Try using: rule add [HOSTNAME] [CHAIN] [POSITION] -j [ACTION] ...\n");
			break;
		case 1:			// remove 0x1, expected args count = 5
			if (countArgs(cmdArgs) > 3) sendMessage(0x15, cmdArgs);
			else printf("Format error. Try using: rule remove [HOSTNAME] [CHAIN] -j [ACTION] ...\n");
			break;
		case 2:			// show 0x2, expected args count = 3
			if(countArgs(cmdArgs) == 3) sendMessage(0x25, cmdArgs);
			else printf("Format error. Try using: rule show [HOSTNAME] [TABLE]\n");
			break;
		case -1:
			printf("Invalid route command. Try using: add, show, remove\n");
			return;
	}
	return;
}

void ruleCommand( char** cmdArgs) {
	int commandIndex = getCommandIndexASR( cmdArgs[1]);
	switch (commandIndex) {
		case 0:			// add 0x0, expected args count = 5
			if (countArgs(cmdArgs) > 3) sendMessage(0x07, cmdArgs);
			else printf("Format error. Try using: rule add [HOSTNAME] from [ADDRESS] table [TABLE NAME]\n");
			break;
		case 1:			// remove 0x1, expected args count = 5
			if (countArgs(cmdArgs) > 3) sendMessage(0x17, cmdArgs);
			else printf("Format error. Try using: rule remove [HOSTNAME] from [ADDRESS] table [TABLE NAME]\n");
			break;
		case 2:			// show 0x2, expected args count = 3
			if(countArgs(cmdArgs) == 3) sendMessage(0x27, cmdArgs);
			else printf("Format error. Try using: rule show [HOSTNAME]\n");
			break;
		case -1:
			printf("Invalid route command. Try using: add, show, remove\n");
			return;
	}
	return;
}

void tableCommand( char** cmdArgs) {
	int commandIndex = getCommandIndexASR( cmdArgs[1]);
	switch (commandIndex) {
		case 0:			// add 0x0, expected args count = 5
			if (countArgs(cmdArgs) == 4) sendMessage(0x06, cmdArgs);
			else printf("Format error. Try using: table add [HOSTNAME] [TABLE NAME]\n");
			break;
		case 1:			// remove 0x1, expected args count = 5
			if (countArgs(cmdArgs) == 4) sendMessage(0x16, cmdArgs);
			else printf("Format error. Try using: route add [HOSTNAME] [TABLE NAME]\n");
			return;
		case 2:			// show 0x2, expected args count = 3
			printf("Invalid route command. Try using: add, remove\n");
			return;
		case -1:
			printf("Invalid route command. Try using: add, remove\n");
			return;
	}
	return;
}

int getCommandIndexASR( char* cmdName) {
	char* commandASR[] = {"add", "remove", "show"};
	int i;
	for( i = 0; i < 3; i++) {
		if( !strcmp( cmdName, commandASR[i])) {
			return i;
		}
	}
	return -1; 			// failed to identify command
}

int countArgs( char** cmdArgs) {
	int i = 0;
	while( cmdArgs[i] != '\0' ) i++;
	return i;
}
