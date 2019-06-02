/*!
 * syscall.c
 * @ System call implementations
 */

#include "syscall.h"

#include "fsdef.h"
#include "lib.h"
#include "memory.h"
#include "page.h"
#include "process.h"
#include "tty.h"
#include "x86_desc.h"

#define ELF_HEADER  0x464C457F

int32_t halt(uint8_t status) {
    cli();

    pcb_t* process = proc0;

    proc0->state = PROC_IDLE;
    proc0 = proc0->parent;

    tty0->nproc--;

    if (proc0 != 0) {
        int32_t pid = proc0->pid;
        proc0->state = PROC_ACTIVE;

        disable_paging();
        map_memory_block(VMEM_USER, PMEM_USER + pid * BLOCK_4MB, USER);
        enable_paging();

        tss.esp0 = KERNEL_BASE - pid * STACK_SIZE;
    }

    sti();

    asm volatile("                          \n\
                 movzbl %0, %%eax           \n\
                 movl   %1, %%esp           \n\
                 movl   %2, %%ebp           \n\
                 movl   %3, 4(%%ebp)        \n\
                 jmp    halt_process        \n\
                 "
                 :
                 : "r" (status),
                   "r" (process->esp),
                   "r" (process->ebp),
                   "r" (process->retaddr)
                );

    return 0;
}

int32_t execute(const int8_t* command) {
    int32_t inode;
    inode = query_inode(command);
    if (inode == -1)
        return -1;

    int8_t header[4];
    if (read_data(inode, 0, header, 4) != 4)
        return -1;

    if (*((uint32_t*)header) != ELF_HEADER)
        return -1;

    int32_t pid = alloc_pid();
    if (pid == PROC_INV)
        return -1;

    cli();

    pcb_t* process = pcb[pid];
    process->parent = proc0;
    process->pid = pid;
    process->state = PROC_ACTIVE;

    proc0 = process;
    tty0->nproc++;

    asm volatile("                      \n\
                 leal   4(%%ebp), %0    \n\
                 movl   %%ebp, %1       \n\
                 movl   4(%%ebp), %2    \n\
                 "
                 : "=rm" (process->esp),
                   "=rm" (process->ebp),
                   "=rm" (process->retaddr)
                );

    process->fds[0] = stdin;
    process->fds[1] = stdout;

    int32_t i;
    for (i = 2; i < FD_MAX; ++i)
        process->fds[i].flags = FD_CLOSE;

    disable_paging();
    map_memory_block(VMEM_USER, PMEM_USER + pid * BLOCK_4MB, USER);
    enable_paging();

    sti();

    read_data(inode, 0, (int8_t*)0x8048000, BLOCK_4MB);
    uint32_t* entry_point = (uint32_t*)(*(uint32_t*)0x8048018);

    tss.ss0 = KERNEL_DS;
    tss.esp0 = KERNEL_BASE - pid * STACK_SIZE;

    asm volatile("                      \n\
                 pushl  $0x002B         \n\
                 pushl  $0x8400000      \n\
                 pushfl                 \n\
                 popl   %%edx           \n\
                 orl    $0x4200, %%edx  \n\
                 pushl  %%edx           \n\
                 pushl  $0x0023         \n\
                 pushl  %0              \n\
                 movw   $0x002B, %%dx   \n\
                 movw   %%dx, %%ds      \n\
                 iret                   \n\
                 halt_process:          \n\
                 "
                 :
                 : "r" (entry_point)
                 : "edx"
                );

    return 0;
}

int32_t read(int32_t fd, void* buf, int32_t nbytes) {
    if (fd >= FD_MAX)
        return -1;

    if (!(proc0->fds[fd].flags & FD_READ))
        return -1;

    return proc0->fds[fd].fops->read(fd, buf, nbytes);
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes) {
    if (fd >= FD_MAX)
        return -1;

    if (!(proc0->fds[fd].flags & FD_WRITE))
        return -1;

    return proc0->fds[fd].fops->write(buf, nbytes);
}

int32_t open(const int8_t* fname) {
    dentry_t* dentry = query_dentry(fname);
    if (dentry == 0)
        return -1;

    int32_t fd;
    for (fd = 2; fd < FD_MAX; ++fd) {
        if (proc0->fds[fd].flags == FD_CLOSE) {
            proc0->fds[fd].flags = FD_OPEN;
            break;
        }
    }

    if (fd == FD_MAX)
        return -1;

    switch (dentry->ftype) {
        case 0:
            /* RTC file */
            break;
        case 1:
            proc0->fds[fd].flags = FD_OPEN | FD_READ;
            proc0->fds[fd].fops = &dir_fops;
            break;
        case 2:
            proc0->fds[fd].flags = FD_OPEN | FD_READ;
            proc0->fds[fd].fops = &file_fops;
            break;
        default:
            return -1;
    }

    proc0->fds[fd].inode = dentry->inode;
    proc0->fds[fd].fpos = 0;

    proc0->fds[fd].fops->open();

    return fd;
}

int32_t close(int32_t fd) {
    if (fd >= FD_MAX)
        return -1;

    if (!(proc0->fds[fd].flags & FD_OPEN))
        return -1;

    proc0->fds[fd].fops->close();
    proc0->fds[fd].flags = FD_CLOSE;

    return 0;
}
