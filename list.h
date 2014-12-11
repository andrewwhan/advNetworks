// node for linked list
typedef struct _packetEntry{
	struct _packetEntry *next;
	char packet[1500];
	int length;
	int tid;
} packetEntry;

void printList( packetEntry* list);
packetEntry* getPacket( packetEntry* list, int tid);
packetEntry* addPacket( packetEntry* list, char packet[1500], int length, int tid);
packetEntry* remPacket( packetEntry* list, int tid);

