void receiveCommand();
int executeArgs(char* args[]);
void addIPv6Alias(char cid, uint tid, short dataLength, char* dataStart);
void removeIPv6Alias(char cid, uint tid, short dataLength, char* dataStart);
void sendFailure(char cid, uint tid);
void sendSuccess(char cid, uint tid);
