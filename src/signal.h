#ifndef SIGNAL_H
#define SIGNAL_H

#include "types.h"

#define SIGINT  0
#define SIGILL  1
#define SIGFPE  2
#define SIGSEGV 3
#define SIGALRM 4
#define SIGUSR1 5
#define SIGUSR2 6
#define NSIG    7

void queue_signal(int32_t pid, uint32_t signum);
void deliver_signal(void);
void invoke_sigreturn(void);

#endif /* SIGNAL_H */
