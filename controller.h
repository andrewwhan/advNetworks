#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

struct hostInfo{
	char hostName[32];
	char secret[16];
	int socket;
	struct hostInfo* next;
};

struct elevConfig{
	char hostName[32];
	char sourceAddr[40];
	char destAddr[40];
	char* action[10];
	struct elevConfig* next;
};

struct hostInfo* loadDatabase();
struct elevConfig* loadElev();
void listenForHosts();
void controllerCommandTerminal();
void parseCommandLine();
void executeUserCommand(char* cmdArgs[32]);
int getCommandIndex(char* cmdName);
int getSocketByName(char* hostName);
char* getNameBySocket(int sockinfo);
void runFile(char** cmdArgs);