#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <string.h>

void controllerCommandTerminal();
void executeUserCommand(char* cmdName, char* cmdArgs[32]);
int getCommandIndex(char* cmdName);