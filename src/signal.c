#include "signal.h"

#include "process.h"
#include "syscall.h"

void queue_signal(int32_t pid, uint32_t signum) {
    if (signum >= NSIG)
        return;

    struct pcb_t* process = pcb[pid];

    uint32_t signal = 0x1 << signum;
    if (process->sigmask & signal)
        return;

    process->sigqueue |= signal;
}

void deliver_signal(void) {
    struct pcb_t* process = proc0;
    uint32_t sigqueue = process->sigqueue;

    if (!sigqueue)
        return;

    uint32_t sigmask = process->sigmask;
    process->sigmask = 0xFFFFFFFF;

    uint32_t signum = 0;
    for (signum = 0; !(sigqueue & 0x1); ++signum)
        sigqueue >>= 1;

    int32_t* handler = process->sighandle[signum];

    if (handler == NULL) {
        switch (signum) {
            case SIGINT:
            case SIGILL:
            case SIGFPE:
            case SIGSEGV:
                halt(-1);
            case SIGALRM:
            case SIGUSR1:
            case SIGUSR2:
            default:
                break;
        }
    } else {
        asm volatile("                              \n\
                     movl   52(%%ebp), %%ecx        \n\
                     subl   $0x8, %%ecx             \n\
                     movl   $0x00000ab8, (%%ecx)    \n\
                     movl   $0x9080cd00, 4(%%ecx)   \n\
                     leal   8(%%ebp), %%eax         \n\
                     subl   $0x34, %%ecx            \n\
                     pushl  $0x34                   \n\
                     pushl  %%eax                   \n\
                     pushl  %%ecx                   \n\
                     call   memcpy                  \n\
                     popl   %%ecx                   \n\
                     addl   $0x8, %%esp             \n\
                     subl   $0x8, %%ecx             \n\
                     movl   %0, 4(%%ecx)            \n\
                     leal   60(%%ecx), %%eax        \n\
                     movl   %%eax, (%%ecx)          \n\
                     movl   %%ecx, 52(%%ebp)        \n\
                     movl   %1, 40(%%ebp)           \n\
                     "
                     :
                     : "r" (signum), "r" (handler)
                     : "eax", "ecx", "edx"
                    );
    }

    process->sigmask = sigmask;
    process->sigqueue &= ~(0x1 << signum);
}
