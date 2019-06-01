/*!
 * process.c
 * @ Functions for process management
 */

#include "process.h"

#include "memory.h"

pcb_t* pcb[PROC_MAX];
pcb_t* proc0;

void init_pcb(void) {
    int32_t i;
    for (i = 0; i < PROC_MAX; ++i) {
        pcb[i] = (pcb_t*)(PROC_BASE - STACK_SIZE * i);
        pcb[i]->state = PROC_IDLE;
    }

    proc0 = 0;
}

int32_t alloc_pid(void) {
    int32_t i;
    for (i = 0; i < PROC_MAX; ++i)
        if (!pcb[i]->state)
            return i;

    return PROC_INV;
}
