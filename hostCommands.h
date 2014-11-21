void receiveCommand(char* messagePtr, int socket);
int addIPv6Alias(char cid, uint tid, short dataLength, char* dataStart);
int removeIPv6Alias(char cid, uint tid, short dataLength, char* dataStart);
int showIPv6Alias(char cid, uint tid, short dataLength, char* dataStart);
int addNatRule(char cid, uint tid, short dataLength, char* dataStart);
int removeNatRule(char cid, uint tid, short dataLength, char* dataStart);
int showNatRule(char cid, uint tid, short dataLength, char* dataStart);
