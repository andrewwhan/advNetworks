void createResendSocket();
void receivePacket( char* msg, int returned, int ctrSock);
int resendElevatedPacket( int tid, char* args);
int dropElevatedPacket(	int tid);
