#include "SocketHandler.h"
#include "HTTPHandler.h"

#include "Config.h"

int send_data(int sock, char *buffer)
{
    int size_recv,total_size= strlen(buffer);
    int num_chunk = total_size / CHUNK_SIZE;
    int offset = 0;
    for(int i=0; i< num_chunk; i++)
    {
		if((size_recv =  send(sock , buffer+offset , CHUNK_SIZE, 0) ) <= 0) return -1;
        offset = offset + size_recv;
    }
    if(offset < total_size)
        if((size_recv =  send(sock , buffer+offset , total_size-offset, 0) ) <= 0) return -1;
	return offset + size_recv;
}


int send_data_fromFile(int sock, char *headerData, FILE *fp)
{
	int n = send_data(sock,headerData);	if(fp==NULL) return n;
	fseek(fp, 0, SEEK_END);
  	long fsize = ftell(fp);
  	fseek(fp, 0, SEEK_SET);

    int size_recv;
    int num_chunk = fsize / CHUNK_SIZE;
    int offset = 0;
	char * buffer = (char*)malloc(CHUNK_SIZE);

    for(int i=0; i< num_chunk; i++)
    {
		fread( buffer, CHUNK_SIZE, 1, fp);
		if((size_recv =  send(sock , buffer , CHUNK_SIZE, 0) ) <= 0) return -1;
        offset = offset + size_recv;
    }
	
    if(offset < fsize)
	{
	fread( buffer, fsize-offset, 1, fp);
    if((size_recv =  send(sock , buffer , fsize-offset, 0) ) <= 0) return -1;
	}
	return offset + size_recv + n;
}


int recv_data(int sock, char *buffer)
{
	int size_recv , total_size= 0;
	while(1)
	{
	
		if((size_recv =  recv(sock , buffer+total_size , CHUNK_SIZE, 0) ) <= 0)return -1;
		total_size += size_recv;
        if(size_recv < CHUNK_SIZE) break;
        if(total_size >= BUFFER_SIZE - 1) break;
	}	
	return total_size;
}

void socketError(int status, char *message)
{
	if(status < 0)
	{
		printf("%s \n", message);
		exit(EXIT_FAILURE);
	}	
}

int createServerSocket()
{
	int serverSockDes;
	socketError((serverSockDes = socket(AF_INET, SOCK_STREAM, 0)),"Error in creating server Socket");
  	struct sockaddr_in serv_addr;
  	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;    
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	serv_addr.sin_port = htons(PORT);
	int opt = 1;	
	socketError(setsockopt(serverSockDes, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT,&opt, sizeof(opt)), "Error setting serversocket option"); 
	socketError(bind(serverSockDes, (struct sockaddr*)&serv_addr,sizeof(serv_addr)),"Error in bind server Socket");
	socketError(listen(serverSockDes, 10), "Failed to listen\n");
	return serverSockDes;
}

int clientState(int clientSockDes, int bytes, int isRead)
{
	if(bytes <= 0)
	{
		printf("[SERVER] client Id %d Exit\n", clientSockDes);
		return -1;
	}
	return 1;	
}

int checkAcceptOkay(int clientSockDes)
{
	if(clientSockDes < 0)
	{
		printf("[SERVER] Client Id %d accept Failed \n", clientSockDes);
		return -1;
	}
	return 1;
}