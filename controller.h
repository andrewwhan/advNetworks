#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void listenForHosts();
void controllerCommandTerminal();
void executeUserCommand(char* cmdArgs[32]);
int getCommandIndex(char* cmdName);
