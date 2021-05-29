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
#include "myOwnthread.h"
#include<pthread.h>
#include "SocketHandler.h"
#include "ThreadQueue.h"
#include "ServerSignals.h"


myThread_mutex gameLock;

extern threadQNode *threadQHead;
myThread_mutex lock;

int serverSockDes = 0;
int activeConnections = 0;


void *handleClient(void *args)
{
    int clientSockDes = *((int*)args);
    printf("[SERVER] Connected to Client Id %d \n", clientSockDes);
    char *request = (char*)malloc(BUFFER_SIZE);
    char *response = NULL;
    int httpVersion = 1; 
    while(1)
    {
        bzero(request, BUFFER_SIZE);
        if(clientState(clientSockDes, recv_data(clientSockDes, request),1) == -1)break;
        // if(clientState(clientSockDes, read(clientSockDes, request, BUFFER_SIZE-1),1) == -1) break;
        Request* req = http_req_parser(request); if(req==NULL) break;
        httpVersion = req->http_ver; 
        Response *res = getResponseObj(req);free_req(req);

        // char *headerData = httpHeaderToString(res);
        // if(clientState(clientSockDes,send_data_fromFile(clientSockDes,headerData, res->fp),0) == -1)
        response = responseToString(res);
 
        free_res(res);
        if(clientState(clientSockDes, write(clientSockDes, response, strlen(response)),0) == -1) break;
	      free(response);
        if(httpVersion == HTTP10){printf("disconnected as http/1.0");break;}   
    }
    free(request);
    close(clientSockDes);
    myThread_mutex_lock(&lock);
    activeConnections--;
    findAndDelete(myThread_self());
    myThread_mutex_unlock(&lock);
    return NULL;
}

int main(void)
{

 int firstInit = 0;
register_server_signals();
 serverSockDes = createServerSocket();
 myThread_mutex_init(&lock);
 myThread_mutex_init(&gameLock);
 
 while(TRUE)
   { 
      socklen_t c;
      myThread tid;
      pthread_t tid1; 
      struct sockaddr_in client_addr;
      int clientSockDes;

      printf("[SERVER] Waiting for clients...\n");
      c = sizeof(struct sockaddr_in);
      
      if(checkAcceptOkay(clientSockDes = accept(serverSockDes, (struct sockaddr*)&client_addr,&c)) == -1) continue;
      myThread_mutex_lock(&lock);
      printf("[SERVER] Active connections %d\n", activeConnections);
      if(activeConnections > THREAD_POOL_SIZE)
      {
        printf("[SERVER] Maximum client limits reached \n");
        printf("[SERVER] Remove client Id %d", threadQHead->threadId);
        if(write(threadQHead->sock, NOT_FOUND_404, strlen(NOT_FOUND_404))> 0)close(threadQHead->sock);
        activeConnections--;
        myThread_cancel(threadQHead->threadId);
        threadQPop();
      }
      activeConnections++;
      myThread_create(&tid,NULL,handleClient,&clientSockDes); 
      // pthread_create(&tid1,NULL,handleClient,&clientSockDes); 
      threadQPush(tid, clientSockDes);
      myThread_mutex_unlock(&lock);
   } 
  close(serverSockDes);
  printf("[SERVER] Excecution Ends\n");
  return 0;
}
