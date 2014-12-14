#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "hostCommands.h"
#include "hostPacket.h"

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

	switch((unsigned char) cid){
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
		case 0x01:
			// Add alias route command
			status = addRoute(cid, tid, dataLength, dataStart);
			break;
		case 0x11:
			// Remove alias route command
			status = removeRoute(cid, tid, dataLength, dataStart);
			break;
		case 0x21:
			// Show alias route command
			status = showRoute(cid, tid, dataLength, dataStart);
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
		case 0x05:
			// Add fire rule command
			status = addFireRule(cid, tid, dataLength, dataStart);
			break;
		case 0x15:
			// Remove fire rule command
			status = removeFireRule(cid, tid, dataLength, dataStart);
			break;
		case 0x25:
			// Show fire rules command
			status = showFireRule(cid, tid, dataLength, dataStart);
			if( status) {
				status = 3; // indicate that it is a show command that executed correctly
			}
			break;
		case 0x06:
			// Add table command
			status = addTable(cid, tid, dataLength, dataStart);
			break;
		case 0x16:
			// Remove table command
			status = removeTable(cid, tid, dataLength, dataStart);
			break;
		case 0x07:
			// Add table rule command
			status = addRule(cid, tid, dataLength, dataStart);
			break;
		case 0x17:
			// Remove table rule command
			status = removeRule(cid, tid, dataLength, dataStart);
			break;
		case 0x27:
			// Show table rules command
			status = showRule(cid, tid, dataLength, dataStart);
			if( status) {
				status = 3; // indicate that it is a show command that executed correctly
			}
			break;
		case 0x08:
			// Add neighbor command
			status = addNeighbor(cid, tid, dataLength, dataStart);
			break;
		case 0x18:
			// Remove neighbor command
			status = removeNeighbor(cid, tid, dataLength, dataStart);
			break;
		case 0x28:
			//Show neighbors command
			status = showNeighbor(cid, tid, dataLength, dataStart);
			if(status){
				status = 3;	// indicate that it is a show command that executed correctly
			}
			break;
		case 0x81:
			// Resend Packet
			status = resendPacket(cid, tid, dataLength, dataStart);
			break;
		case 0x82:
			// Drop Packet
			printf("Droppin this \n");
			status = dropPacket(cid, tid, dataLength, dataStart);
			break;
		case 0x0A:
			// Exit command
			cleanExit(socket, messagePtr);
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

/* Command Specific Function Template
void function(char cid, uint tid, short dataLength, char* dataStart){
	
}
*/

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

int addNeighbor(char cid, uint tid, short dataLength, char* dataStart) {
	char** cmdtok = tokenizeData(dataStart);

	char* args[32] = {"ip", "neigh", "replace", cmdtok[3], "lladdr", cmdtok[4], "dev", "eth0", '\0' };

	int success = executeArgs(args);

	free(cmdtok);
	return success;
}

int removeNeighbor(char cid, uint tid, short dataLength, char* dataStart) {
	int success;
	char** cmdtok = tokenizeData(dataStart);
	
	if(cmdtok[4] != NULL && cmdtok[3] != NULL) {

		char* args[32] = {"ip", "neigh", "del", cmdtok[3], "lladdr", cmdtok[4], "dev", "eth0", '\0' };
		success = executeArgs(args);
	} else {
		success = 0;
	}

	free(cmdtok);
	return success;
}

int showNeighbor(char cid, uint tid, short dataLength, char* dataStart) {

	char* args[32] = { "ip", "neigh", "show", "nud", "permanent", "nud", "reachable", "nud", "stale", '\0' };

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
	char** cmdtok = tokenizeData(dataStart);

	char* args[32] = { "ip", "-6", "route", "show", "table", cmdtok[3], '\0' };

	int success = executeShow(args);

	free(cmdtok);
	return success;
}

int addFireRule(char cid, uint tid, short dataLength, char* dataStart) {
	char* cmdtok [32];
	const char* delimiter = " \n";
	int tokcnt = 1;
	int success;

	cmdtok[0] = strtok( dataStart, delimiter);			// tokenize command
	while( cmdtok[tokcnt-1]){
		cmdtok[tokcnt] = strtok( NULL, delimiter);
		tokcnt++;
	}

	char* args[32] = { "ip6tables", "-I", cmdtok[3], cmdtok[4], cmdtok[5], cmdtok[6] };
	int i = 6;
	while (i < tokcnt) {
		args[i] = cmdtok[i+1];
		i++;
	}
	args[i] == '\0';
	
	success = executeArgs(args);
	return success;
}

int removeFireRule(char cid, uint tid, short dataLength, char* dataStart) {
	char* cmdtok [32];
	const char* delimiter = " \n";
	int tokcnt = 1;
	int success;

	cmdtok[0] = strtok( dataStart, delimiter);			// tokenize command
	while( cmdtok[tokcnt-1]){
		cmdtok[tokcnt] = strtok( NULL, delimiter);
		tokcnt++;
	}

	char* args[32] = {"ip6tables", "-D", cmdtok[3], cmdtok[4], cmdtok[5] };
	int i = 6;
	while (i < tokcnt) {
		args[i] = cmdtok[i+1];
		i++;
	}
	args[i] == '\0';
	
	success = executeArgs(args);
	return success;
}

int showFireRule(char cid, uint tid, short dataLength, char* dataStart) {

	char* args[32] = { "ip6tables", "-L", '\0' };

	int success = executeShow(args);

	return success;
}

int addTable(char cid, uint tid, short dataLength, char* dataStart) {

	char** cmdtok = tokenizeData(dataStart);

	FILE* tableFile;
	FILE* newTableFile;
	tableFile = fopen("/etc/iproute2/rt_tables", "r");
	//tableFile = fopen("rt_tables2", "r");
	
	char output[50][128];
	int argCount = 0;

	int end = 0;
	while(argCount < 11) {		// get data from output file
		if(fgets(output[argCount], 128, tableFile) != NULL){
			printf("%d, %s \n", argCount, output[argCount]);
			argCount++;
		}
	}
	
	int tableId = 0;
	printf("tableId: %d\n", tableId);
	char name[128];
	char tab[2] = {'	', '\0'};
	char endLine[2] = {'\n', '\0'};
	end = fscanf(tableFile, "%d %s", &tableId, name);
	printf("tableId: %d\n", tableId);
	while(end != EOF){			// keep adding extra tables
		output[argCount][0] = '\0';
		sprintf(output[argCount], "%d", tableId);
		strcat(output[argCount], tab);
		strcat(output[argCount], name);
		strcat(output[argCount], endLine);
		printf("%s", output[argCount]);
		end = fscanf(tableFile, "%d %s", &tableId, name);
		printf("Table: %d, %s \n", tableId, name);
		argCount++;
	}
	fclose(tableFile);

	tableId++;
	printf("tableId: %d\n", tableId);
	sprintf(output[argCount], "%d", tableId);
	strcat(output[argCount], tab);
	strcat(output[argCount], cmdtok[3]);
	strcat(output[argCount], endLine);
	printf("Hier %s \n", output[11]);
	printf("Hi %s \n", output[argCount]);

	newTableFile = fopen("/etc/iproute2/rt_tables", "w");

	int i=0;
	while(i < argCount+1) {		// rewrite the file with new data
		printf("%s", output[i]);
		fputs(output[i], newTableFile);
		i++;
	}
	
	fclose(newTableFile);
	free(cmdtok);
	return 1;
}

int removeTable(char cid, uint tid, short dataLength, char* dataStart) {

	char** cmdtok = tokenizeData(dataStart);

	char* args[32] = { "ip", "route", "flush", "table", cmdtok[3], '\0' };
	
	int success = executeShow(args);

	free(cmdtok);
	return success;
}

int addRule(char cid, uint tid, short dataLength, char* dataStart) {
	char* cmdtok [32];
	const char* delimiter = " \n";
	int tokcnt = 1;
	int success;

	cmdtok[0] = strtok( dataStart, delimiter);			// tokenize command
	while( cmdtok[tokcnt-1]){
		cmdtok[tokcnt] = strtok( NULL, delimiter);
		tokcnt++;
	}

	printf("%s, %s\n", cmdtok[5], cmdtok[6]);
	char* args[32] = {"ip", "-6", "rule", "add", cmdtok[3], cmdtok[4], cmdtok[5], cmdtok[6], '\0' };
/*	int i = 6;
	while (i < tokcnt) {
		args[i] = cmdtok[i+1];
		i++;
	}
	args[i] == '\0';*/
	
	success = executeArgs(args);
	return success;
}

int removeRule(char cid, uint tid, short dataLength, char* dataStart) {
	char* cmdtok [32];
	const char* delimiter = " \n";
	int tokcnt = 1;
	int success;

	cmdtok[0] = strtok( dataStart, delimiter);			// tokenize command
	while( cmdtok[tokcnt-1]){
		cmdtok[tokcnt] = strtok( NULL, delimiter);
		tokcnt++;
	}

	char* args[32] = {"ip", "-6", "rule", "del", cmdtok[3], cmdtok[4], cmdtok[5], cmdtok[6], '\0' };
	/*int i = 6;
	while (i < tokcnt) {
		args[i] = cmdtok[i+1];
		i++;
	}
	args[i] == '\0';*/
	
	success = executeArgs(args);
	return success;
}

int showRule(char cid, uint tid, short dataLength, char* dataStart) {
	char** cmdtok = tokenizeData(dataStart);

	char* args[32] = { "ip", "-6", "rule", "show", '\0' };

	int success = executeShow(args);

	free(cmdtok);
	return success;
}

int resendPacket(char cid, uint tid, short dataLength, char* dataStart){
	char** args = { '\0' };
	return !resendElevatedPacket( tid, dataStart);
}

int dropPacket(char cid, uint tid, short dataLength, char* dataStart) {

	return !dropElevatedPacket( tid);
}

void cleanExit(int socket, char* messagePtr){
	close(socket);
	free(messagePtr);
	exit(0);
}
