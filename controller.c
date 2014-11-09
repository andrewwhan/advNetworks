#include "controller.h"
#include "commands.h"
#include "hostCommands.h"

uint nextTid = 512;

struct hostInfo{
	char hostName[32];
	char secret[16];
	int socket;
	struct hostInfo* next;
};

int main( int argc, char* argv[]){
<<<<<<< HEAD
	struct hostInfo* hosts = loadDatabase(); //Read database file for host information
	//listenForHosts(hosts); // listen to establish connections to hosts
=======
	struct hostInfo* hosts = loadDatabase();	//Read database file for host information
	listenForHosts();							// listen to establish connections to hosts
>>>>>>> 0a2fcbd0fd8486e7da726647be270753434a4b44
	
	controllerCommandTerminal();				// start command line for user input
}

struct hostInfo* loadDatabase(){
	FILE* dbFile;
	dbFile = fopen("database.txt", "r");
	int end = 0;
	struct hostInfo* firstHost = NULL;
	struct hostInfo* prevHost = NULL;
	while(end != EOF) {
		struct hostInfo* host = malloc(sizeof(struct hostInfo));
		if(firstHost == NULL){
			firstHost = host;
			prevHost = host;
		} else {
			prevHost->next = host;
		}
		
		end = fscanf(dbFile, "%s %s", host->hostName, host->secret);
		printf("name: %s,\n secret: %s,\n end: %d\n", host->hostName, host->secret, end); 

	}
	// Catches the last iteration of the loop where there is no data
	free(prevHost->next);
	prevHost->next = NULL;
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
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	int commSocket;
	int waitHosts = 1; //If there are still unconnected hosts
	while(waitHosts){
		listen(listenSocket, 5);
		addr_size = sizeof(their_addr);
		commSocket = accept(listenSocket, (struct sockaddr *)&their_addr, &addr_size);
		char* msg = malloc(1500*sizeof(char));
		int returned = recv(commSocket, msg, 1500, 0);
		if(returned > 0){
			char cid = *msg;
			uint tid = *(msg+1);
			short dataLength = *(msg+5);
			if(cid == 0x04){
				char hostName[32];
				char secret[16];
				sscanf(msg+7, "%s %s", hostName, secret);
				struct hostInfo* currentHost = firstHost;
				int hostFound = 0;
				while(currentHost != NULL){
					if(!strcmp(hostName, currentHost->hostName)){
						if(!strcmp(secret, currentHost->secret)){
							currentHost->socket = commSocket;
							hostFound = 1;
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
							break;
						}
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
	close(listenSocket);
	return;
}

void controllerCommandTerminal() {
	char cmdline [130];
	const char* delimiter = " \n";
	char* cmdtok [32];
	const char* cdstr = "cd";
	const char* exitstr = "exit";

	while(1){ 								// start command loop
		printf( "(^_^)> ");
		fgets( cmdline, 129, stdin);
		if( strlen( cmdline) < 128){ 		// ensure command is within certain length
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
			printf("Please limit lines to 128 characters. \n");
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
