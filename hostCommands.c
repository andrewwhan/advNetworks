#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "hostCommands.h"

#define HEADER 7

void receiveCommand(char* messagePtr, int socket) {
	char cid = *messagePtr;
	uint tid = *(uint*)(messagePtr + 1);
	short dataLength = *(short*)(messagePtr + 5);
	char* dataStart = messagePtr + 7;

	printf("cid:		%02X\n", cid);
	printf("tid:		%u\n", tid);
	printf("dataLength:	%hd\n", dataLength);
	printf("dataStart:	%s\n", dataStart);
	int i;

	int status;

	switch(cid){
		case 0x00:
			// Add alias command
			status = addIPv6Alias(cid, tid, dataLength, dataStart);
			break;
		case 0x10:
			// Remove alias command
			status = removeIPv6Alias(cid, tid, dataLength, dataStart);
			break;
		case 0x20:
			// Show aliases command
			status = showIPv6Alias(cid, tid, dataLength, dataStart);
			if( status) {
				status = 3; // indicate that it is a show command that executed correctly
			}
			break;
		case 0x32:
			// Request response from host command
			status = !0; // send success response
			break;
		case 0x03:
			// Add nat rule command
			status = addNatRule(cid, tid, dataLength, dataStart);
			break;
		case 0x13:
			// Remove nat rule command
			status = removeNatRule(cid, tid, dataLength, dataStart);
			break;
		case 0x23:
			// Show nat rules command
			status = showNatRule(cid, tid, dataLength, dataStart);
			if( status) {
				status = 3; // indicate that it is a show command that executed correctly
			}
			break;
		case 0x01:
			// Add route command
			status = addRoute(cid, tid, dataLength, dataStart);
			break;
		case 0x11:
			// Remove route command
			status = removeRoute(cid, tid, dataLength, dataStart);
			break;
		case 0x21:
			// Show route command
			status = showRoute(cid, tid, dataLength, dataStart);
			if( status) {
				status = 3; // indicate that it is a show command that executed correctly
			}
			break;
		default:
			break;
	}
	if(status == 3) {
		sendShow(cid, tid, socket);
	} else if (status) {
		sendSuccess(cid, tid, socket);
	} else {
		sendFailure(cid, tid, socket);
	}
	return;
}

/* Command Specific Function Template
void function(char cid, uint tid, short dataLength, char* dataStart){
	
}
*/

char** tokenizeData(char* dataStart){
	char** cmdtok = (char**) malloc(sizeof(char*)*32);
	const char* delimiter = " \n";
	int tokcnt = 1;

	cmdtok[0] = strtok( dataStart, delimiter);			// tokenize command
	while( cmdtok[tokcnt-1]){
		cmdtok[tokcnt] = strtok( NULL, delimiter);
		tokcnt++;
	}
	return cmdtok;
}

int addIPv6Alias(char cid, uint tid, short dataLength, char* dataStart) {
	char** cmdtok = tokenizeData(dataStart);

	char* args[32] = {"ifconfig", cmdtok[4], "inet6", "add", cmdtok[3], '\0' };

	int success = executeArgs(args);

	free(cmdtok);
	return success;
}

int removeIPv6Alias(char cid, uint tid, short dataLength, char* dataStart) {
	char** cmdtok = tokenizeData(dataStart);

	char* args[32] = {"ifconfig", cmdtok[4], "inet6", "del", cmdtok[3], '\0' };
	int success = executeArgs(args);

	free(cmdtok);
	return success;
}

int showIPv6Alias(char cid, uint tid, short dataLength, char* dataStart) {

	char* args[32] = { "ifconfig", '\0' };

	int success = executeShow(args);

	return success;
}

int addNatRule(char cid, uint tid, short dataLength, char* dataStart) {
	char** cmdtok = tokenizeData(dataStart);

	char* args[32] = {"ip6tables", "-t", "nat", "-I", cmdtok[3], cmdtok[4], "-p", "tcp",
		cmdtok[5], cmdtok[6], cmdtok[7], cmdtok[8], cmdtok[9], cmdtok[10], '\0' };

	int success = executeArgs(args);

	free(cmdtok);
	return success;
}

int removeNatRule(char cid, uint tid, short dataLength, char* dataStart) {
	char** cmdtok = tokenizeData(dataStart);
	
	char* args[32] = {"ip6tables", "-t", "nat", "-D", cmdtok[3], "-p", "tcp",
		cmdtok[4], cmdtok[5], cmdtok[6], cmdtok[7], cmdtok[8], cmdtok[9], '\0' };

	int success = executeArgs(args);

	free(cmdtok);
	return success;
}

int showNatRule(char cid, uint tid, short dataLength, char* dataStart) {

	char* args[32] = { "ip6tables", "-t", "nat", "-L", '\0' };

	int success = executeShow(args);

	return success;
}

int addRoute(char cid, uint tid, short dataLength, char* dataStart) {
	char* cmdtok [32];
	const char* delimiter = " \n";
	int tokcnt = 1;

	cmdtok[0] = strtok( dataStart, delimiter);			// tokenize command
	while( cmdtok[tokcnt-1]){
		cmdtok[tokcnt] = strtok( NULL, delimiter);
		tokcnt++;
	}

	char* args[32] = {"ip", "-6", "route", "add"};
	int i = 4;
	while (i < tokcnt) {
		args[i] = cmdtok[i-1];
		i++;
	}
	args[i] == '\0';

	int success = executeArgs(args);

	return success;
}

int removeRoute(char cid, uint tid, short dataLength, char* dataStart) {
	char* cmdtok [32];
	const char* delimiter = " \n";
	int tokcnt = 1;
	int success;

	cmdtok[0] = strtok( dataStart, delimiter);			// tokenize command
	while( cmdtok[tokcnt-1]){
		cmdtok[tokcnt] = strtok( NULL, delimiter);
		tokcnt++;
	}

	char* args[32] = {"ip", "-6", "route", "del"};
	int i = 4;
	while (i < tokcnt) {
		args[i] = cmdtok[i-1];
		i++;
	}
	args[i] == '\0';
	
	success = executeArgs(args);
	return success;
}

int showRoute(char cid, uint tid, short dataLength, char* dataStart) {

	char* args[32] = { "route", "-A", "inet6", '\0' };

	int success = executeShow(args);

	return success;
}
