int executeArgs(char* args[]);
int executeShow(char* args[]);
void showResponse( char* msg);
void sendFailure(char cid, uint tid, int socket);
void sendSuccess(char cid, uint tid, int socket);
void sendShow(char cid, uint tid, int socket);
