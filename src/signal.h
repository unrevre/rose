#ifndef SIGNAL_H
#define SIGNAL_H

#include "types.h"

#define SIGNONE 0
#define SIGINT  1
#define SIGILL  2
#define SIGFPE  3
#define SIGSEGV 4
#define SIGALRM 5
#define SIGUSR1 6
#define SIGUSR2 7
#define NSIG    8

void queue_signal(uint32_t signum);
void deliver_signal(void);
void invoke_sigreturn(void);

#endif /* SIGNAL_H */
