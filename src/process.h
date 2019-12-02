/*!
 * process.h
 * @ Defines and function definitions for process management
 */

#ifndef PROCESS_H
#define PROCESS_H

#include "fd.h"
#include "signal.h"
#include "tty.h"
#include "types.h"

#define FD_INV      -1
#define FD_MAX      8

typedef struct pcb_t pcb_t;

struct pcb_t {
    int32_t pid;
    uint32_t state;
    uint32_t esp;
    uint32_t ebp;
    uint32_t retaddr;
    int8_t args[64];
    uint32_t sigmask;
    uint32_t sigqueue;
    int32_t* sighandle[NSIG];
    uint32_t task_esp;
    uint32_t task_ebp;
    tty_t* tty;
    pcb_t* parent;
    fd_t fds[FD_MAX];
};

#define PROC_INV    -1
#define PROC_MAX    4

#define PROC_FREE   0
#define PROC_SLEEP  1
#define PROC_ACTIVE 2

extern pcb_t* pcb[PROC_MAX];
extern pcb_t* proc0;

void init_pcb(void);

int32_t alloc_pid(void);

#endif /* PROCESS_H */
