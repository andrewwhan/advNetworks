#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

struct hostInfo;
struct hostInfo* loadDatabase();
void listenForHosts();
void controllerCommandTerminal();
void parseCommandLine();
void executeUserCommand(char* cmdArgs[32]);
int getCommandIndex(char* cmdName);
int getSocketByName(char* hostName);
void runFile(char** cmdArgs);