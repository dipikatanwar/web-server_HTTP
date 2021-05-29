#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "myOwnthread.h"
#include "Queue.h"
#include "alarm.h"

extern struct node* readyQ;
TCB* RUNNING_TCB = NULL;

void handle_alarm_signal(int signum, siginfo_t *info, void *context)
{
  if (RUNNING_TCB == NULL && readyQ == NULL) {
     printf("[myThread] Exit As both RUNNING_TCB and readyQ Empty \n");
	  _exit(EXIT_SUCCESS);
    }

    // printf("[myThread]TCB Executing by timer-------- %d \n", RUNNING_TCB->tid);

    TCB *this = RUNNING_TCB;
    if(popFirstReadyFromQueue(&readyQ,&RUNNING_TCB)==0)
    {
	    RUNNING_TCB = this;
    }
    else if(this == NULL)
    {
      setcontext(&RUNNING_TCB->context);
    }
    else
    {
      pushToQueue(&readyQ, this);
      swapcontext(&this->context, &RUNNING_TCB->context);
    }
}



void start_func(void)
{
    block_sigalrm();
    TCB *this = RUNNING_TCB;
    unblock_sigalrm();    
    void *result = this->threadFun(this->arg);
    myThread_exit(result);
}

TCB* makeTcb(myThread_attr* attr,void*(*func)(),void *args)
{
  
  int static tcbCount = 0;
  TCB* tcb = (TCB*)malloc(sizeof(TCB));
  if(tcb==NULL) return NULL;
  
  tcb->tid= tcbCount++;
  tcb->ret = NULL;
  tcb->threadFun = func;
  tcb->arg = args;
  tcb->state = READY;
  tcb->waitingForJoin = NULL;

  getcontext(&tcb->context);

  if(func == NULL){ return tcb; }

  if(attr == NULL)
  {
    (tcb->context).uc_stack.ss_sp = malloc(STACK_SIZE);
    (tcb->context).uc_stack.ss_size = STACK_SIZE;
  }
  else
  {
    (tcb->context).uc_stack.ss_sp = attr->stackAddr;
    (tcb->context).uc_stack.ss_size = attr->stackSize;
  }
  makecontext(&tcb->context,start_func, 0);
  return tcb;
}


void myThread_create(myThread *ptr,myThread_attr *attr,void* (*func)(),void *args)
{


   int static firstInit = 0;   
   if(firstInit == 0)
   {
    	 TCB* tcb = makeTcb(NULL,NULL,NULL); //First main thread
       RUNNING_TCB = tcb;
      //  printf("[myThread]----------  MAIN tid %d------------\n", RUNNING_TCB->tid);
       start_clock();
       firstInit++;
   }   

  block_sigalrm();
  int *tt = (void*)malloc(sizeof(args));
  *tt = *(int*)args;

  //printf("[MyThread make tcb ] %d %p\n", *tt, tt);

   TCB* tcb = makeTcb(attr,func,(void*)tt);
   *ptr = tcb->tid;

   
   pushToQueue(&readyQ,tcb);
   unblock_sigalrm();
}

myThread myThread_self(){ return RUNNING_TCB->tid; }

void myThread_yield()
{
  // printf("--------Yield thread id %d \n", RUNNING_TCB->tid);
  block_sigalrm();
  TCB *this = RUNNING_TCB;
  if(popFirstReadyFromQueue(&readyQ, &RUNNING_TCB)==0)
  {
    printf("[myThread]---Current thread is last thread in ready queue \n");
    return;
  }
  if(pushToQueue(&readyQ,this)==0)
  {
    printf("[myThread]--- Error in pushing element to queue \n");
	  exit(EXIT_SUCCESS);
  }
  swapcontext(&this->context, &RUNNING_TCB->context); 
  unblock_sigalrm();
}

void myThread_exit(void *result)
{
    block_sigalrm();
    TCB *this = RUNNING_TCB;
    if(RUNNING_TCB == NULL) {
	    printf("[myThread] RUNNUNG TCB NULL\n");
	    exit(EXIT_SUCCESS);
      }

    if(this->waitingForJoin != NULL)this->waitingForJoin->state = READY;
    this->ret = result;
    free(this->context.uc_stack.ss_sp); 

    if(popFirstReadyFromQueue(&readyQ, &RUNNING_TCB)==0)
    {
	    printf("[myThread]  %d\n", this->tid);
	    exit(EXIT_SUCCESS);
    }
//    printf("[myThread] Exit \n");
    unblock_sigalrm();
    setcontext(&RUNNING_TCB->context);
    // swapcontext(&this->context, &RUNNING_TCB->context); 

}

int myThread_join(myThread tid,void **ret)
{
    block_sigalrm();
    TCB *t = getTCBwithTid(readyQ, tid);
    if(t==NULL || t->waitingForJoin != NULL)
    {
      unblock_sigalrm();
      return -1;
    }
    t->waitingForJoin = RUNNING_TCB;
    RUNNING_TCB->state = WAITING;
    RUNNING_TCB->ret = t->ret; 
    myThread_yield();
    unblock_sigalrm();
    return 1;
}

void myThread_cancel(myThread tid)
{
  block_sigalrm();
  TCB *t = popTCBwithTid(&readyQ, tid);
  if(t==NULL)
  {
    unblock_sigalrm();
    return;
  }

  if(t->waitingForJoin != NULL)t->waitingForJoin->state = READY;
  t->ret = 0;
  free(t->context.uc_stack.ss_sp);
  free(t); 
  unblock_sigalrm();
  return;
}

void myThread_test()
{
 while(popFirstReadyFromQueue(&readyQ, &RUNNING_TCB) != 0)
      printf("%d ", RUNNING_TCB->tid);
}
// void myThreadswitch();

void start_clock()
{

// sigset_t all;
// sigfillset(&all);

sigset_t all;
sigemptyset(&all);
sigaddset(&all, SIGALRM);

const struct sigaction alarm = {
.sa_sigaction = handle_alarm_signal,
.sa_mask = all,
.sa_flags = SA_SIGINFO | SA_RESTART
};

sigaction(SIGALRM, &alarm, NULL);

struct itimerval value;
value.it_interval.tv_sec = 0;        /* Zero seconds */
value.it_interval.tv_usec = 50000;  /* Fifty milliseconds */
value.it_value.tv_sec = 0;           /* Zero seconds */
value.it_value.tv_usec = 50000;     /* Five hundred milliseconds */
setitimer(ITIMER_REAL, &value, NULL);
}

//############################## Functions Related to Synchronization ######################


int myThread_mutex_init(myThread_mutex *m)
{
    m->locked_by = -1;
    m->value = 1;
    return 1;
}

int myThread_mutex_lock(myThread_mutex *m)
{
    while(1)
    {
        int r = 1;
        if(atomic_compare_exchange_strong(&m->value,&r,0) == 1)
        {
            m->locked_by = (RUNNING_TCB==NULL)?0:RUNNING_TCB->tid;
            // printf("Locked %d\n", RUNNING_TCB->tid);
            return 1;
        }
    }
}


int myThread_mutex_unlock(myThread_mutex *m)
{
    if(m->locked_by != RUNNING_TCB->tid){
        printf("Same thread should unlock, which locked mutex");
        _exit(EXIT_SUCCESS);
    }
    int r = 0;
    if(atomic_compare_exchange_strong(&m->value, &r,1) == 1)
    {
        m->locked_by = -1;
        return 1;
    }
    else
    {
        return 0;
    }

}


int myThread_sem_init(myThread_sem *m, int value)
{
    m->value = value;
    return 1;
}


int myThread_sem_lock(myThread_sem *m)
{
    while(1)
    {
        int r = 1;
        if(atomic_compare_exchange_strong(&m->value,&r,0) == 1)
        {
            return 1;
        }
    }
}


int myThread_sem_unlock(myThread_sem *m)
{
    int r = 0;
    if(atomic_compare_exchange_strong(&m->value, &r,1) == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
