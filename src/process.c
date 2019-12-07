/*!
 * process.c
 * @ Functions for process management
 */

#include "process.h"

#include "memory.h"

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

int32_t alloc_pid(void) {
    int32_t i;
    for (i = 0; i < PROC_MAX; ++i)
        if (!pcb[i]->state)
            return i;

    return PROC_INV;
}
