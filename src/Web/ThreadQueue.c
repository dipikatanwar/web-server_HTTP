#include<stdio.h>
#include "ThreadQueue.h"
#include <stdlib.h>

threadQNode *threadQHead = NULL;
threadQNode *threadQTail = NULL;

threadQNode *makeThreadQNode(int threadId, int sock)
{
    threadQNode *t = (threadQNode*)malloc(sizeof(threadQNode));
    if(t==NULL) return NULL;
    t->threadId = threadId;
    t->sock = sock;
    t->next = NULL;
    return t;
}

int threadQPush(int threadId, int sock)
{
    threadQNode *t = makeThreadQNode(threadId,sock); if(t==NULL) return -1;
    if(threadQHead == NULL && threadQTail == NULL)
    {
        threadQHead = threadQTail = t;
    }
    else
    {
        threadQTail->next = t;
    }
    return 1;
}

int threadQPop()
{
    if(threadQHead == NULL) return -1;
    threadQNode *t = threadQHead;
    if(threadQHead == threadQTail)
    {
        threadQHead = threadQTail = NULL;
    }
    else
    {
        threadQHead = threadQHead->next; 
    }
    free(t);
    return 1;     
}

threadQNode* threadQFront()
{
    return threadQHead;
}

int isEmpty()
{
    return threadQHead == NULL;
}

int findAndDelete(int threadId)
{
    if(threadQHead == NULL) return -1;
    if(threadQHead->threadId == threadId) return threadQPop();
    threadQNode *t;
    threadQNode *itr = threadQHead;
    while(itr->next != NULL)
    {
        t = itr->next;
        if(t->threadId == threadId)
        {
            itr->next = t->next;
            free(t);
            return 1;
        }
        itr = itr->next;
    }
    return -1;      
}