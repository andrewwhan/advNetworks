#include "controller.h"
#include "commands.h"
#include "hostCommands.h"

uint nextTid = 512;

int main( int argc, char* argv[]){
	listenForHosts(); // listen to establish connections to hosts
	
	controllerCommandTerminal(); // start command line for user input
}

void listenForHosts(){

}

void controllerCommandTerminal() {
	char cmdline [130];
	const char* delimiter = " \n";
	char* cmdtok [32];
	const char* cdstr = "cd";
	const char* exitstr = "exit";

	while(1){ 							// start command loop
		printf( "(^_^)> ");
		fgets( cmdline, 129, stdin);
		if( strlen( cmdline) < 128){ 		// ensure command is within certain length
			if( feof( stdin)){									// for piping in file
				printf( "exit\n");
				exit( 0);
			}
			int tokcnt = 1;
			cmdtok[0] = strtok( cmdline, delimiter);				// tokenize command
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
					executeUserCommand(cmdtok);		// execute the user command
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
