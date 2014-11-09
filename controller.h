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
void executeUserCommand(char* cmdArgs[32]);
int getCommandIndex(char* cmdName);
