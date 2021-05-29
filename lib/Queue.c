#include<stdio.h>
#include "myOwnthread.h"
#include "Queue.h"

struct node *readyQ = NULL;
struct node *waitQ = NULL;


struct node * makeNode(TCB *tcb)
{
	struct node * t = (struct node*)malloc(sizeof(struct node));
	if(t==NULL) return NULL;
	t->tcb = tcb;
	t->next=t->prev=t;
	return t;
}

void printQ(struct node *qHead)
{
	printf("Pringing Q \n");
	if(qHead == NULL) {
		printf("Qheaf null");
		return;
	}
	printf("00000000000000 : ");

	struct node *iter = qHead;
	do
	{
		printf("%d ", iter->tcb->tid);
		iter = iter->next;
	} while (iter != qHead);
	printf("\n");
};

int pushToQueue(struct node **qH, TCB*tcb)
{
	struct node *qHead = *qH;
	//printf("push id %d \n", tcb->tid);
	struct node *t = makeNode(tcb);
	if(t==NULL) 
	{
		*qH = NULL;
		return 0;
	}

	if(qHead==NULL)
	{
		qHead = t;
	}
	else
	{
  		t->next = qHead;
  		t->prev = qHead->prev;
  		qHead->prev = t;
  		t->prev->next = t;
	}
	*qH = qHead;
  	return 1;
}

int popFromQueue(struct node **qH, TCB**tcb)
{
	struct node *qHead = *qH;
	*tcb = NULL;
  	if(qHead==NULL)
	{
		*qH = NULL; 
		return 0;
	}
  	struct node *t = qHead;
  	
	if(qHead->next != qHead)
  	{
    	qHead = qHead->next;
    	qHead->prev = t->prev;
    	qHead->prev->next = qHead;
  	}
	else
  	{
    	qHead = NULL;
  	}

   *tcb = t->tcb;
   free(t);
   *qH = qHead;
   return 1;
}

int popFirstReadyFromQueue(struct node **qH, TCB**tcb)
{
	struct node *qHead = *qH;
	if(qHead == NULL) 
	{
		*qH = NULL;
		return 0;
	}
	if(qHead->tcb->state == READY) return popFromQueue(qH, tcb);
	if(qHead == qHead->next) return 0;
	
	for(struct node *iter = qHead->next; iter != qHead; iter = iter->next)
	{
		if(iter->tcb->state == READY)
		{
			*qH = iter;
			 return popFromQueue(qH, tcb);
		}
	}
	return 0;

}



TCB *popTCBwithTid(struct node ** qH, myThread tid)
{
	TCB *t = NULL; 
	struct node *qHead = *qH;
	if(qHead==NULL) return NULL;
	if(qHead->next == qHead && qHead->tcb->tid != tid) return NULL;
	
	if(qHead->tcb->tid == tid)
	{
		popFromQueue(qH,&t);
	}
	else
	{
		for(struct node *iter = qHead->next; iter != qHead; iter = iter->next)
		{
			if(iter->tcb->tid == tid)
			{
				popFromQueue(&iter, &t);
			}
		}

	}
	return t;
}


TCB *getTCBwithTid(struct node * qHead, myThread tid)
{
	TCB *t = NULL; 
	if(qHead==NULL) return NULL;
	if(qHead->next == qHead && qHead->tcb->tid != tid) return NULL;
	
	if(qHead->tcb->tid == tid)
	{
		t = qHead->tcb;
	}
	else
	{
		for(struct node *iter = qHead->next; iter != qHead; iter = iter->next)
		{
			if(iter->tcb->tid == tid)
			{
				t = iter->tcb;
				break;
			}
		}

	}
	return t;
}

int isWatingForMoreToComplete(struct node *qHead, TCB *tcb)
{
	return 0;
}

int destroy(struct node *qHead)
{
	struct node *t;
	if(qHead==NULL) return 0;
	while(qHead)
	{
		t = qHead;
		qHead = qHead->next;
		free(t->tcb);
		free(t);
	}
	return 1;
}

TCB * getTcb(struct node *qHead, int tid)
{
	TCB *t = NULL;
	struct node *iter;
	if(qHead == NULL) return t;

	if(qHead->tcb->tid==tid)
	{
		t = qHead->tcb;
		return t;
	}

	iter = qHead->next;
	while(iter != qHead)
	{
		if(iter->tcb->tid == tid)
		{
			t = iter->tcb;
			return t;
		}
		iter = iter->next;
	}
	return t;
}



/*

int main()
{
	for(int i=0; i< 2; i++)
	{
		TCB *x = (TCB*)malloc(sizeof(TCB));
		x->tid = i;
		pushToQueue(&readyQ, x);
	}

	printQ(readyQ);
	TCB *y;

	popFromQueue(&readyQ,&y);
	printf("%d\n",y->tid );

	pushToQueue(&readyQ, y);

	printQ(readyQ);
	popFromQueue(&readyQ,&y);
	printf("%d\n",y->tid);

	popFromQueue(&readyQ,&y);
	printf("%d\n",y->tid);

	// int pp = popFromQueue(&readyQ, &y);
	// printf("%d %d\n",y->tid, pp);
	// while(popFromQueue(&readyQ, &y)==1)
	// {
	// 	printf("%d", y->tid);
	// }
	return 0;
}

*/
