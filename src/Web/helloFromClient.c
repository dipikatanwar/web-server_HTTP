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

int createClientSocket()
{
  int clientSockDes;
	socketError((clientSockDes = socket(AF_INET, SOCK_STREAM, 0)),"Error in creating client Socket\n");
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
	socketError(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr),"Invalid address/ Address not supported \n");
	socketError(connect(clientSockDes, (struct sockaddr *)&serv_addr, sizeof(serv_addr)),"Connect Invalid address/ Address not supported \n");
  return clientSockDes;
}

int main(void)
{
buff = (char*)malloc(BUFFER_SIZE);
clock_t start, end;
char *helloMessage = "HelloWebServer";
int clientSockDes;
clientSockDes = createClientSocket();
bzero(buff, BUFFER_SIZE);
sprintf(buff,"GET /hello?clientId=%d&message=%s HTTP/1.0\n\n",clientSockDes,helloMessage);
printf("Request Message %s \n", buff);
if(clientState(clientSockDes, write(clientSockDes, buff, strlen(buff)),0) == -1) return -1;

bzero(buff,BUFFER_SIZE);
if(clientState(clientSockDes, recv_data(clientSockDes,buff),1) == -1) return -1;
printf("Response Message %s \n", buff);

close(clientSockDes);
return 0;

}
