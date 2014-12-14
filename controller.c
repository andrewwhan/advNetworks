#include "controller.h"
#include "commands.h"
#include "elevator.h"
#include <errno.h>

uint nextTid = 1351;
struct hostInfo* firstHost;
struct elevConfig* elevData;

int main( int argc, char* argv[]){
	firstHost = loadDatabase();	//Read database file for host information
	elevData = loadElev(); //Read elevation database file
	//printf("First %s followed by %s \n", hosts->hostName, hosts->next->hostName);
	listenForHosts();					// listen to establish connections to hosts
	controllerCommandTerminal();				// start command line for user input
}

struct hostInfo* loadDatabase(){
	FILE* dbFile;
	dbFile = fopen("database.txt", "r");
	int end = 0;
	struct hostInfo* firstHost = NULL;
	struct hostInfo* prevHost = NULL;
	while(end != EOF) {
		char hostName[32];
		char secret[16];
		end = fscanf(dbFile, "%s %s", hostName, secret);
		if(end != EOF){
			struct hostInfo* host = malloc(sizeof(struct hostInfo));
			if(firstHost == NULL){
				firstHost = host;
				prevHost = host;
			} else {
				prevHost->next = host;
				prevHost = host;
			}
			memcpy(host->hostName, hostName, 32);
			memcpy(host->secret, secret, 16);
			host->socket = 0;
		}
		else{
			prevHost->next = NULL;
		}
	}
	fclose(dbFile);
	return firstHost;
}

struct elevConfig* loadElev(){
	FILE* elevFile;
	elevFile = fopen("elevConfig.txt", "r");
	int end = 0;
	struct elevConfig* firstRule = NULL;
	struct elevConfig* prevRule = NULL;

	while(end != EOF){
		char hostName[32];
		char sourceAddr[40];
		char destAddr[40];
		char* action[10];

		fscanf(elevFile, "%s", hostName);
		if(fscanf(elevFile, " source=%[^,],", sourceAddr) == 0){
			sourceAddr[0] = '*';
			sourceAddr[1] = '\0';
		}
		if(fscanf(elevFile, " dest=%[^,],", destAddr) == 0){
			destAddr[0] = '*';
			destAddr[1] = '\0';
		}
		end = fscanf(elevFile, " action=(%m[^)]),", action);
		if(end != EOF){
			struct elevConfig* rule = malloc(sizeof(struct elevConfig));
			if(firstRule == NULL){
				firstRule = rule;
				prevRule = rule;
			}
			else{
				prevRule->next = rule;
				prevRule = rule;
			}
			const char* delimiter = ";";
			int tokcnt = 1;
			rule->action[0] = strtok( action[0], delimiter);			// tokenize command
			while( rule->action[tokcnt-1]){
				rule->action[tokcnt] = strtok( NULL, delimiter);
				tokcnt++;
			}
			memcpy(rule->hostName, hostName, 32);
			memcpy(rule->sourceAddr, sourceAddr, 40);
			memcpy(rule->destAddr, destAddr, 40);
			printf("%s %s %s %s \n", hostName, sourceAddr, destAddr, action[0]);
		}
		else{
			prevRule->next = NULL;
		}

	}
	fclose(elevFile);
	return firstRule;
}

void listenForHosts(){
	char* port = "3875";
	struct addrinfo hints, *res;
	int listenSocket;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	int status = getaddrinfo(NULL, port, &hints, &res);
	if(status != 0){
		printf("getaddrinfo error: %s\n", gai_strerror(status));
		return;
	}
	listenSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(listenSocket == -1){
		printf("Socket error \n");
		return;
	}
	if(bind(listenSocket, res->ai_addr, res->ai_addrlen) == -1){
		printf("Bind error %s\n", strerror(errno));
		close(listenSocket);
		return;
	}
	printf("Socket set up \n");
	struct sockaddr_storage hostAddr;
	socklen_t addrSize;
	int commSocket;
	int waitHosts = 1; //If there are still unconnected hosts
	while(waitHosts){
		fd_set inputs;
		int numfds;
		FD_ZERO(&inputs);
		FD_SET(0, &inputs); //stdin
		FD_SET(listenSocket, &inputs); //listen socket
		numfds = listenSocket;
		struct hostInfo* currentHost = firstHost;
		while(currentHost != NULL){
			if(currentHost->socket != 0){
				FD_SET(currentHost->socket, &inputs);
				if(currentHost->socket > numfds){
					numfds = currentHost->socket;
				}
			}
			currentHost = currentHost->next;
		}
		select(numfds+1, &inputs, NULL, NULL, NULL);
		if(FD_ISSET(0, &inputs)){
			char cmdline[514];
			fgets( cmdline, 512, stdin);
			parseCommandLine(cmdline);
			printf(">> ");
			fflush(stdout);
		}
		if(FD_ISSET(listenSocket, &inputs)){
			listen(listenSocket, 5);
			addrSize = sizeof(hostAddr);
			commSocket = accept(listenSocket, (struct sockaddr *)&hostAddr, &addrSize);
			char* msg = malloc(1500*sizeof(char));
			int returned = recv(commSocket, msg, 1500, 0);
			if(returned > 0){
				char cid = *msg;
				uint tid = *(msg+1);
				short dataLength = *(msg+5);
				if(cid == 0x04){
					char hostName[32];
					char secret[16];
					memset(secret, 0, 16);
					sscanf(msg+7, "%s %s", hostName, secret);
					struct hostInfo* currentHost = firstHost;
					int hostFound = 0;
					while(currentHost != NULL){
						if(!strcmp(hostName, currentHost->hostName)){
							if(!strcmp(secret, currentHost->secret)){
								currentHost->socket = commSocket;
								hostFound = 1;
								printf("Host %s connected! \n", hostName);
							}
							else{
							//Invalid secret
							}
							break;
						}
						currentHost = currentHost->next;
					}
					if(hostFound){ //If the host was located check if there are still unconnected hosts
						waitHosts = 0;
						currentHost = firstHost;
						while(currentHost != NULL){
							if(currentHost->socket == 0){
								waitHosts = 1;
								printf(">> ");
								fflush(stdout);
								break;
							}
							currentHost = currentHost->next;
						}
					}
					else{
						close(commSocket);
					}
				}
				else{
					close(commSocket);
				}
			}
			free(msg);
		}
		while(currentHost != NULL){
			if(currentHost->socket != 0){
				if(FD_ISSET(currentHost->socket, &inputs)){
					elevate(currentHost->socket);
				}
			}
			currentHost = currentHost->next;
		}
	}
	printf("All hosts connected! \n");
	close(listenSocket);
	return;
}

void controllerCommandTerminal() {
	char cmdline [514];
	while(1){ 								// start command loop
		printf( ">> ");
		fflush(stdout);
		fgets( cmdline, 512, stdin);
		parseCommandLine(cmdline);
	}
	exit(0);
}

void parseCommandLine(char* cmdline){
	const char* exitstr = "exit";
	const char* delimiter = " \n";
	char* cmdtok [32];
	if( strlen( cmdline) < 513){ 		// ensure command is within certain length
		if( feof( stdin)){									// for piping in file
			printf( "exit\n");
			exit( 0);
		}
		int tokcnt = 1;
		cmdtok[0] = strtok( cmdline, delimiter);			// tokenize command
		while( cmdtok[tokcnt-1]){
			cmdtok[tokcnt] = strtok( NULL, delimiter);
			tokcnt++;
		}
		if( tokcnt <= 32){									// ensure valid number of arguments
			if(!cmdtok[0]){
			}
			else if( !strcmp( cmdtok[0], exitstr)){
				struct hostInfo* currentHost = firstHost;
				while(currentHost != NULL){
					if(currentHost->socket != 0){
						char* exitArgs[2];
						exitArgs[0] = malloc(sizeof(char)*5);
						strcpy(exitArgs[0], "exit");
						exitArgs[1] = currentHost->hostName;
						strncpy(exitArgs[1], currentHost->hostName, 32);
						executeUserCommand(exitArgs);
						free(exitArgs[0]);
					}
					// close(currentHost->socket);
					currentHost = currentHost->next;
				}
				currentHost = firstHost;
				while(currentHost != NULL){
					currentHost = currentHost->next;
					free(firstHost);
					firstHost = currentHost;
				}
				struct elevConfig* currentRule = elevData;
				while(currentRule != NULL){
					currentRule = currentRule->next;
					free(elevData->action[0]);
					free(elevData);
					elevData = currentRule;
				}
				exit(0);
			}
			else{
				executeUserCommand(cmdtok);					// execute the user command
			}
		}
		else{
			printf("Please limit input to 32 arguments. \n");
		}
	}
	else{
		printf("Please limit lines to 512 characters. \n");
		char c = getc(stdin);
		while(c != '\n' && c != EOF){
			c = getc(stdin);
		}
	}
}

void executeUserCommand( char* cmdArgs[32]) {
	if( !cmdArgs[1]){
		printf( "no second command\n");					// error: no second command
	} else {
		int indexOfCommand = getCommandIndex( cmdArgs[0]);
		switch ( indexOfCommand) {
			case 0:										// alias command
				aliasCommand( cmdArgs);
				break;
			case 1:										// request command
				responseCommand( cmdArgs);
				break;
			case 2:										// nat command
				natCommand( cmdArgs);
				break;
			case 3:
				exitCommand(cmdArgs);
				break;
			case 4:
				runFile(cmdArgs);
				break;
			case 5:
				neighCommand(cmdArgs);
				break;
			case 6:
				routeCommand(cmdArgs);
				break;
			case 7:
				fireruleCommand(cmdArgs);
				break;
			case 8:
				ruleCommand(cmdArgs);
				break;
			case 9:
				tableCommand(cmdArgs);
				break;
			case -1:									// error: command name not recognized
			printf( "not valid command\n");
			break;
		}
	}
	return;
}

int getCommandIndex( char* cmdName) {
	const char* cmdNames[] = { "alias", "request", "nat", "exit", "file", "neigh", "route", "firerule", "rule", "table"};			// check command names
	int i;
	for( i = 0; i < 10; i++) {
		if( !strcmp( cmdName, cmdNames[i])) {
			return i;
		}
	}
	return -1; 			// failed to identify command
}

int getSocketByName(char* hostName){
	struct hostInfo* currentHost = firstHost;
	while(currentHost != NULL){
		if(!strcmp(hostName, currentHost->hostName)){
			return currentHost->socket;
		}
		currentHost = currentHost->next;
	}
	return -1;
}

char* getNameBySocket(int sockinfo){
	struct hostInfo* currentHost = firstHost;
	while(currentHost != NULL){
		if(sockinfo == currentHost->socket){
			return currentHost->hostName;
		}
		currentHost = currentHost->next;
	}
	return NULL;
}

void runFile(char** cmdArgs){
	FILE* toRun;
	if((toRun = fopen(cmdArgs[1], "r")) != NULL){
		int fileLeft = 1;
		while(fileLeft){
			char cmdLine[514];
			if(fgets(cmdLine, 512, toRun) != 0){
				parseCommandLine(cmdLine);
			}
			else{
				fileLeft = 0;
			}
		}
		fclose(toRun);
	}
	else{
		printf("%s \n", strerror(errno));
	}
}
