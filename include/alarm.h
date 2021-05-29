#ifndef ALARM_H
#define ALARM_H
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

void block_sigalrm(void);
void unblock_sigalrm(void);

#endif