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
    int32_t pid = proc0->pid;

    int32_t i;
    for (i = 0; i < PROC_MAX; ++i) {
        pid = (pid + 1) & (PROC_MAX - 1);
        if (pcb[pid]->state == PROC_ACTIVE)
            return pid;
    }

    return -1;
}

void raise(int32_t pid) {
    struct pcb_t* process = proc0;
    struct pcb_t* target = pcb[pid];

    if (process != NULL) {
        asm volatile("                      \n\
                     movl   %%ebp, %0       \n\
                     "
                     : "=rm" (process->ebp)
                    );
    }

    if (process == target)
        return;

    cli();

    disable_paging();
    map_memory_block(VMEM_USER, PMEM_USER + pid * BLOCK_4MB, USER);

    struct tty_t* tty = target->tty;
    map_video_memory((tty0 == tty) ? PMEM_VIDEO : tty_buffer(tty));
    enable_paging();

    sti();

    tss.esp0 = KERNEL_BASE - pid * STACK_SIZE;

    proc0 = target;

    asm volatile("                      \n\
                 movl   %0, %%ebp       \n\
                 leave                  \n\
                 ret                    \n\
                 "
                 :
                 : "rm" (target->ebp)
                );
}
