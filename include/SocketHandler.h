#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "HTTPHandler.h"
#include "Config.h"
#include <signal.h>


extern int serverSockDes;

int send_data(int sock, char *buffer);

int recv_data(int sock, char *buffer);

int send_data_fromFile(int sock, char *headerData, FILE *fp);

void socketError(int status, char *message);

int createServerSocket();

void register_signals();

int clientState(int clientSockDes, int bytes, int isRead);

int checkAcceptOkay(int clientSockDes);

#endif