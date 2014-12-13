#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"

/*
 * print the list of neighbors
 */
void printPackets( packetEntry* list) {

	if( list != NULL){
		printf("\npacket\ntid: %d\nlength: %d\n", list->tid, list->length);
		fwrite(list->packet, list->length, 1, stdout);
		printPackets( list->next);
	}
}

/*
 * get a packet of a certain tid
 */
packetEntry* getPacket( packetEntry* list, int tid) {

	if(	list->tid == tid) {
		return list;
	} else if( list == NULL) {
		return list;
	} else {
		return getPacket( list->next, tid);
	}
}

/*
 * Add a neighbor to a given list
 */
packetEntry* addPacket( packetEntry* list, char packet[1500], int length, int tid) {

	if( list == NULL) {
		list = (packetEntry*) malloc( sizeof( packetEntry));
		list->next = NULL;
		list->length = length;
		list->tid = tid;
		memcpy(list->packet, packet, length);
		return list;
	} else {
		list->next = addPacket( list->next, packet, length, tid);
		return list;
	}
}

/*
 * remove the top neighbor from the list
 */
packetEntry* remPacket( packetEntry* list, int tid) {
	
	if(list == NULL) {
		return list;
	}else if( list->tid == tid) {
		packetEntry* ret = list->next;
		free(list);
		return ret;
	} else { 
		list->next = remPacket( list->next, tid);
		return list;
	}
}
