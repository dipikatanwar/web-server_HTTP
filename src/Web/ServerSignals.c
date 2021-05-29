#include "ServerSignals.h"

void sighandler(int signum) 
{
   printf("[Server] Killed with signal  %d , Exiting Safely\n", signum);
   if(serverSockDes != 0) close(serverSockDes);
   exit(signum);
}

void register_server_signals()
{
 	signal(SIGTSTP, sighandler); 
  	signal(SIGINT, sighandler);
}