/*!
 * schedule.c
 * @ System scheduling
 */

#include "schedule.h"

#include "lib.h"
#include "memory.h"
#include "page.h"
#include "process.h"
#include "tty.h"
#include "x86_desc.h"

int32_t schedule_next(void) {
    pcb_t* process = proc0;

    if (process == NULL)
        return -1;

    int32_t pid = process->pid + 1;

    int32_t i;
    for (i = 1; i < PROC_MAX; ++i, ++pid) {
        pid = pid & (PROC_MAX - 1);
        if (pcb[pid]->state == PROC_ACTIVE)
            return pid;
    }

    return -1;
}

void raise(int32_t pid) {
    pcb_t* process = proc0;
    pcb_t* target = pcb[pid];

    if (process != NULL) {
        asm volatile("                      \n\
                     movl   %%esp, %0       \n\
                     movl   %%ebp, %1       \n\
                     "
                     : "=rm" (process->task_esp),
                       "=rm" (process->task_ebp)
                    );
    }

    cli();

    disable_paging();
    map_memory_block(VMEM_USER, PMEM_USER + pid * BLOCK_4MB, USER);

    tty_t* tty = target->tty;
    map_video_memory((tty0 == tty) ? PMEM_VIDEO : tty_buffer(tty));
    enable_paging();

    tss.esp0 = KERNEL_BASE - pid * STACK_SIZE;

    sti();

    proc0 = target;

    asm volatile("                      \n\
                 movl   %0, %%esp       \n\
                 movl   %1, %%ebp       \n\
                 leave                  \n\
                 ret                    \n\
                 "
                 :
                 : "rm" (target->task_esp),
                   "rm" (target->task_ebp)
                );
}
