/*!
 * process.c
 * @ Functions for process management
 */

#include "process.h"

#include "memory.h"
#include "tty.h"

struct pcb_t* pcb[PROC_MAX];
struct pcb_t* proc0;

void init_pcb(void) {
    int32_t i;
    for (i = 0; i < PROC_MAX; ++i) {
        pcb[i] = (struct pcb_t*)(PROCESS_BASE - i * STACK_SIZE);
        pcb[i]->state = PROC_FREE;
    }

    proc0 = NULL;
}

void init_pid0(void) {
    struct pcb_t* process = pcb[0];

    process->pid = 0;
    process->state = PROC_ACTIVE;
    process->sigmask = 0xFFFFFFFF;
    process->tty = tty0;
    process->parent = NULL;

    proc0 = process;
}

int32_t alloc_pid(void) {
    int32_t i;
    for (i = 0; i < PROC_MAX; ++i)
        if (pcb[i]->state == PROC_FREE)
            return i;

    return PROC_INV;
}
