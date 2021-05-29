#ifndef MYTHREAD_H
#define MYTHREAD_H
#include<stdio.h>
#include<ucontext.h>
#include <stdatomic.h>

#define READY 1
#define WAITING 2
#define STACK_SIZE (4*1024)


typedef struct mtcb
{
	int tid;
	ucontext_t context;
	void* (*threadFun)(void*);
	struct mtcb * waitingForJoin;
	void *arg;
	int state;
	void *ret;
} TCB;


typedef int myThread;

typedef struct
{
	int stackSize;
	void *stackAddr;
}myThread_attr;





void myThread_create(myThread *ptr,myThread_attr *attr,void* (*func)(),void *args);
void myThread_exit(void *result);
void myThread_cancel(myThread tid);
void myThread_yield(void);
int myThread_join(myThread tid,void **ret);
myThread myThread_self();
void myThread_test();


void myThread_attr_init(myThread_attr* attr);
void myThread_attr_setstacksize(myThread_attr* attr,int size);
void mythread_attr_getstacksize(myThread_attr* attr,int* size);
void myThread_attr_setstackaddr(myThread_attr* attr,void* addr);
void myThread_attr_getstackaddr(myThread_attr* attr,void** addr);
void myThread_attr_destroy(myThread_attr* attr);
void myThreadswitch();

/* Alarm Function */
void start_clock();
void block_sigalrm(void);
void unblock_sigalrm(void);
void handle_thread_start(void);


//############################## Functions Related to Synchronization ######################


typedef struct 
{
	int value;
	int locked_by;
} myThread_mutex;

int myThread_mutex_init(myThread_mutex *m);
int myThread_mutex_lock(myThread_mutex *m);
int myThread_mutex_unlock(myThread_mutex *m);


typedef struct 
{
	int value;
} myThread_sem;

int myThread_sem_init(myThread_sem *m, int value);
int myThread_sem_lock(myThread_sem *m);
int myThread_sem_unlock(myThread_sem *m);


#endif
