#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include<signal.h>
#include "SocketHandler.h"
#include <sys/time.h>
#include <time.h>
#include "Config.h"

char *buff;
int clientSockDes;

void sighandler(int signum) 
{
   printf("[Client] Killed with signal  %d , Exiting Safely\n", signum);
   if(clientSockDes != 0) close(clientSockDes);
   free(buff);
   exit(signum);
}

int createClientSocket()
{
  int clientSockDes;
	socketError((clientSockDes = socket(AF_INET, SOCK_STREAM, 0)),"Error in creating client Socket\n");
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
	socketError(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr),"Invalid address/ Address not supported \n");
	socketError(connect(clientSockDes, (struct sockaddr *)&serv_addr, sizeof(serv_addr)),"Invalid address/ Address not supported \n");
  return clientSockDes;
}

double data[20][2];
int file_size[20];

int main(void)
{
  int sockfd = 0,n = 0;
signal(SIGTSTP, sighandler); 
signal(SIGINT, sighandler);
  
buff = (char*)malloc(BUFFER_SIZE);
clock_t start, end;

int fileName = 1;
int clientSockDes;

// 10 HTTP/1.0
//11 HTTP/1.1
int type = 10;
while(type!=12)
{
fileName = 1;
while(1)
{
        start = clock();
        if(type == 10 || fileName == 1) 
        {
          // printf("client connected \n");
          clientSockDes = createClientSocket();
        }
        
        if(fileName == 10) break; 
        bzero(buff, BUFFER_SIZE);
        sprintf(buff,"%s %s%d%s %s\n\n", "GET","/data/",fileName,".txt" ,(type==10)?"HTTP/1.0":"HTTP/1.1");
        // printf("%s \n", buff);
        
        if(clientState(clientSockDes, write(clientSockDes, buff, strlen(buff)),0) == -1) break;
        // if(clientState(clientSockDes, send_data(clientSockDes, buff),0) == -1)
        // {
        //   printf("Error in send data \n");
        //    break;
        // }
        bzero(buff,BUFFER_SIZE);
        if(clientState(clientSockDes, recv_data(clientSockDes,buff),1) == -1) break;
        end = clock();
        double timeused = ((double) (end - start) / CLOCKS_PER_SEC);
	//printf("%ld\t%lf\n",strlen(buff),timeused);
        file_size[fileName-1] = strlen(buff);
        data[fileName-1][type -10] = timeused;
        fileName +=1;
	if(type == 10) close(clientSockDes);
}
type += 1;
close(clientSockDes);
}

  FILE *fp = fopen("./data/sizeVsHTTPVer.txt", "w");

  fprintf(fp,"%s","SIZE  HTTP/1.0 HTTP/1.1 \n");
  printf("SIZE  HTTP/1.0 HTTP/1.1 \n");
  for(int i = 0; i< fileName - 1; i++)
  {
    fprintf(fp,"%d   %lf  %lf \n", file_size[i], data[i][0], data[i][1]);
    printf("%d   %lf  %lf \n", file_size[i], data[i][0], data[i][1]);
  }  
  // close(fp);

  return 0;
}
