#ifndef THREAD_QUEUE
#define THREAD_QUEUE


typedef struct threadQNode
{
    int threadId;
    int sock;
    struct threadQNode *next;

}threadQNode;


threadQNode *makeThreadQNode(int threadId, int sock);

int threadQPush(int threadId, int sock);
int threadQPop();
int findAndDelete(int threadId);

#endif