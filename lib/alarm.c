
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>


void block_sigalrm(void)
{
    sigset_t all;
    sigemptyset(&all);
    sigaddset(&all, SIGALRM);
    sigprocmask(SIG_BLOCK, &all, NULL);
}


void unblock_sigalrm(void)
{
    sigset_t all;
    sigemptyset(&all);
    sigaddset(&all, SIGALRM);
    sigprocmask(SIG_UNBLOCK, &all, NULL);
}




/*
int main()
{
start_clock();
int i = 0;
for(i=0; i < 1000000000; i++);
printf("block \n");
block_sigalrm();

for(i=0; i < 1000000000; i++);
printf("unblock \n");

unblock_sigalrm();

for(i=0; i < 1000000000; i++);
printf("block \n");

block_sigalrm();

return 0;
}
*/