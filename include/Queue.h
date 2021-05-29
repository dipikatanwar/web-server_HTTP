#ifndef QUEUE_H
#define QUEUE_H
#include<stdio.h>
#include<malloc.h>
#include "myOwnthread.h"

struct node
{
	TCB *tcb;
	struct node *next;
	struct node *prev;

};

void printQ(struct node *qHead);
int pushToQueue(struct node **qHead, TCB*tcb);
int popFromQueue(struct node **qHead, TCB**tcb);
TCB *popTCBwithTid(struct node ** qH, myThread tid);
TCB *getTCBwithTid(struct node * qHead, myThread tid);
int popFirstReadyFromQueue(struct node **qH, TCB**tcb);
// int popFirstReady(TCB** tcb);

int destroy(struct node *qHead);
TCB * getTcb(struct node *qHead, int tid);
#endif
