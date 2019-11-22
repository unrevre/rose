#include "signal.h"

#include "lib.h"
#include "process.h"
#include "syscall.h"

void queue_signal(uint32_t signum) {
    if (proc0->sigmask)
        return;

    if (signum >= NSIG)
        return;

    proc0->signum = signum;
}

void deliver_signal(void) {
    proc0->sigmask = 1;

    uint32_t signum = proc0->signum;
    int32_t* handler = proc0->sighandle[signum];

    if (!handler) {
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
                asm volatile("jmp   _after");
        }
    }

    asm volatile("                          \n\
                 leal   8(%%ebp), %%eax     \n\
                 movl   44(%%eax), %%ecx    \n\
                 subl   $0x8, %%ecx         \n\
                 pushl  $0x8                \n\
                 leal   _sigreturn, %%edx   \n\
                 pushl  %%edx               \n\
                 pushl  %%ecx               \n\
                 call   memcpy              \n\
                 subl   $0x34, %%ecx        \n\
                 addl   $0xc, %%esp         \n\
                 pushl  $0x34               \n\
                 pushl  %%eax               \n\
                 pushl  %%ecx               \n\
                 call   memcpy              \n\
                 addl   $0xc, %%esp         \n\
                 subl   $0x8, %%ecx         \n\
                 movl   %0, 4(%%ecx)        \n\
                 leal   60(%%ecx), %%edx    \n\
                 movl   %%edx, (%%ecx)      \n\
                 movl   %%ecx, 44(%%eax)    \n\
                 movl   %1, 32(%%eax)       \n\
                 jmp    _after              \n\
                 _sigreturn:                \n\
                 movl   $0xa, %%eax         \n\
                 int    $0x80               \n\
                 nop                        \n\
                 _after:                    \n\
                 "
                 :
                 : "r" (signum), "r" (handler)
                 : "eax", "ecx", "edx"
                );

    proc0->sigmask = 0;
    proc0->signum = SIGNONE;
}
