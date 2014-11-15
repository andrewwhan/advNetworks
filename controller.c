#include "controller.h"
#include "commands.h"

uint nextTid = 512;

struct hostInfo{
	char hostName[32];
	char secret[16];
	int socket;
	struct hostInfo* next;
};

int main( int argc, char* argv[]){
	struct hostInfo* hosts = loadDatabase();	//Read database file for host information
	printf("First %s followed by %s \n", hosts->hostName, hosts->next->hostName);
	//listenForHosts(hosts);					// listen to establish connections to hosts

	
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
			printf("name: %s,\n secret: %s,\n end: %d\n", host->hostName, host->secret, end); 
		}
		else{
			prevHost->next = NULL;
		}
	}
	return firstHost;
}

void listenForHosts(struct hostInfo* firstHost){
	char* port = "3875";
	struct addrinfo hints, *res;
	int listenSocket;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
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
		printf("Bind error \n");
		close(listenSocket);
		return;
	}
	printf("Socket set up \n");
	struct sockaddr_storage hostAddr;
	socklen_t addrSize;
	int commSocket;
	int waitHosts = 1; //If there are still unconnected hosts
	while(waitHosts){
		listen(listenSocket, 5);
		addrSize = sizeof(hostAddr);
		commSocket = accept(listenSocket, (struct sockaddr *)&hostAddr, &addrSize);
		char* msg = malloc(1500*sizeof(char));
		int returned = recv(commSocket, msg, 1500, 0);
		if(returned > 0){
			printf("Message Received, cid %02X \n", *msg);
			char cid = *msg;
			uint tid = *(msg+1);
			short dataLength = *(msg+5);
			if(cid == 0x04){
				char hostName[32];
				char secret[16];
				memset(secret, 0, 16);
				sscanf(msg+7, "%s %s", hostName, secret);
				printf("secret: %s \n", secret);
				struct hostInfo* currentHost = firstHost;
				printf("Hostname : %s \n Secret : %s \n", hostName, secret);
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
						printf("Breaking");
						break;
					}
					currentHost = currentHost->next;
				}
				if(hostFound){ //If the host was located check if there are still unconnected hosts
					waitHosts = 0;
					currentHost = firstHost;
					while(currentHost != NULL){
						if(currentHost->socket == 0){
							printf("Host not connected yet: %s \n", currentHost->hostName);
							waitHosts = 1;
							break;
						}
						printf("Iterating past %s to %s \n", currentHost->hostName, currentHost->next->hostName);
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
		printf("Loop end \n");
	}
	close(listenSocket);
	return;
}



void controllerCommandTerminal() {
	char cmdline [514];
	const char* delimiter = " \n";
	char* cmdtok [32];
	const char* cdstr = "cd";
	const char* exitstr = "exit";

	while(1){ 								// start command loop
		printf( "(^_^)> ");
		fgets( cmdline, 512, stdin);
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
	exit(0);
}

void executeUserCommand( char* cmdArgs[32]) {
	if( !cmdArgs[0]){
		printf( "no second command\n");					// error: no second command
	} else {
		int indexOfCommand = getCommandIndex( cmdArgs[0]);
		switch ( indexOfCommand) {
			case 0:										// alias command
				printf( "alias command\n");
				aliasCommand( cmdArgs);
				break;
			case 1:										// request command
				printf( "request response command\n");
				responseCommand( cmdArgs);
				break;
			case 2:										// nat command
				printf( "nat command\n");
				natCommand( cmdArgs);
				break;
			case -1:									// error: command name not recognized
				printf( "not valid command\n");
				break;
		}
	}
	return;
}

int getCommandIndex( char* cmdName) {
	const char* cmdNames[] = { "alias", "request", "nat"};			// check command names
	int i;
	for( i = 0; i < 3; i++) {
		if( !strcmp( cmdName, cmdNames[i])) {
			return i;
		}
	}
	return -1; 			// failed to identify command
}
