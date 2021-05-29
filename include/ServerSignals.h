#ifndef SERVER_SIGNALS
#define SERVER_SIGNALS
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

void register_server_signals();

#endif