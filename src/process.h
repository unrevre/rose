/*!
 * process.h
 * @ Defines and function definitions for process management
 */

#ifndef PROCESS_H
#define PROCESS_H

#include "fd.h"
#include "signal.h"
#include "types.h"

#define FD_INV      -1
#define FD_MAX      8

struct pcb_t {
    int32_t pid;
    uint32_t state;
    uint32_t ebp;
    uint32_t retaddr;
    int8_t args[64];
    uint32_t sigmask;
    uint32_t sigqueue;
    int32_t* sighandle[NSIG];
    uint32_t task_ebp;
    struct tty_t* tty;
    struct pcb_t* parent;
    struct fd_t fds[FD_MAX];
};

#define PROC_INV    -1
#define PROC_MAX    8

#define PROC_FREE   0
#define PROC_SLEEP  1
#define PROC_ACTIVE 2

extern struct pcb_t* pcb[PROC_MAX];
extern struct pcb_t* proc0;

void init_pcb(void);
void init_pid0(void);

int32_t alloc_pid(void);

#endif /* PROCESS_H */
