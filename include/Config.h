#ifndef CONFIG_H
#define CONFIG_H
#define PORT 8888
#define BUFFER_SIZE 1024*128
#define THREAD_POOL_SIZE 10
#define CHUNK_SIZE 256


#define TRUE 1
#define FALSE 0

#define PRINTSTR(str)   printf("[SERVER] %s \n", str);

#define SITE_ROOT  "./site_root"

#define CHAT_LOGS  "./site_root/chat/logs.txt"

// const char * SERVER_LOG_DIR = "";


#endif
