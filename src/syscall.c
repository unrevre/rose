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
    pcb_t* parent = process->parent;

    process->state = PROC_IDLE;

    tty0->nproc--;

    if (parent != 0) {
        int32_t pid = parent->pid;
        parent->state = PROC_ACTIVE;

        disable_paging();
        map_memory_block(VMEM_USER, PMEM_USER + pid * BLOCK_4MB, USER);
        enable_paging();

        tss.esp0 = KERNEL_BASE - pid * STACK_SIZE;
    }

    proc0 = parent;

    sti();

    asm volatile("                          \n\
                 movzbl %0, %%eax           \n\
                 movl   %1, %%esp           \n\
                 movl   %2, %%ebp           \n\
                 movl   %3, 4(%%ebp)        \n\
                 jmp    halt_process        \n\
                 "
                 :
                 : "rm" (status),
                   "rm" (process->esp),
                   "rm" (process->ebp),
                   "rm" (process->retaddr)
                 : "eax"
                );

    return 0;
}

int32_t execute(const int8_t* command) {
    int8_t binary[64];
    strncpy(binary, command, 64);
    binary[63] = '\0';

    int32_t i;
    for (i = 0; i < 63; ++i) {
        if (binary[i] == ' ' || binary[i] == '\0') {
            binary[i++] = '\0';
            break;
        }
    }

    int32_t inode;
    inode = query_inode(binary);
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

    pcb_t* parent = proc0;
    pcb_t* process = pcb[pid];

    process->parent = parent;
    process->pid = pid;
    process->state = PROC_ACTIVE;

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

    strncpy(process->args, command + i, 64 - i);

    process->sigmask = 0;
    process->signum = 0;
    memset(process->sighandle, 0, NSIG * sizeof(int32_t*));

    process->fds[0] = stdin;
    process->fds[1] = stdout;

    for (i = 2; i < FD_MAX; ++i)
        process->fds[i].flags = FD_CLOSE;

    disable_paging();
    map_memory_block(VMEM_USER, PMEM_USER + pid * BLOCK_4MB, USER);
    enable_paging();

    proc0 = process;

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

    pcb_t* process = proc0;

    if (!(process->fds[fd].flags & FD_READ))
        return -1;

    return process->fds[fd].fops->read(fd, buf, nbytes);
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes) {
    if (fd >= FD_MAX)
        return -1;

    pcb_t* process = proc0;

    if (!(process->fds[fd].flags & FD_WRITE))
        return -1;

    return process->fds[fd].fops->write(buf, nbytes);
}

int32_t open(const int8_t* fname) {
    dentry_t* dentry = query_dentry(fname);
    if (dentry == 0)
        return -1;

    pcb_t* process = proc0;

    int32_t fd;
    for (fd = 2; fd < FD_MAX; ++fd) {
        if (process->fds[fd].flags == FD_CLOSE) {
            process->fds[fd].flags = FD_OPEN;
            break;
        }
    }

    if (fd == FD_MAX)
        return -1;

    switch (dentry->ftype) {
        case 0:
            process->fds[fd].flags = FD_OPEN | FD_READ | FD_WRITE;
            process->fds[fd].fops = &rtc_fops;
            break;
        case 1:
            process->fds[fd].flags = FD_OPEN | FD_READ;
            process->fds[fd].fops = &dir_fops;
            break;
        case 2:
            process->fds[fd].flags = FD_OPEN | FD_READ;
            process->fds[fd].fops = &file_fops;
            break;
        default:
            return -1;
    }

    process->fds[fd].inode = dentry->inode;
    process->fds[fd].fpos = 0;

    process->fds[fd].fops->open();

    return fd;
}

int32_t close(int32_t fd) {
    if (fd >= FD_MAX)
        return -1;

    pcb_t* process = proc0;

    if (!(process->fds[fd].flags & FD_OPEN))
        return -1;

    process->fds[fd].fops->close();
    process->fds[fd].flags = FD_CLOSE;

    return 0;
}

int32_t getargs(int8_t* buf, int32_t nbytes) {
    pcb_t* process = proc0;

    if (strnlen(process->args, 64) > nbytes)
        return -1;

    strncpy(buf, process->args, nbytes);

    return 0;
}

int32_t vidmap(uint8_t** address) {
    return -1;
}

int32_t signal(int32_t signum, void* handler) {
    if (!signum || signum >= NSIG)
        return -1;

    proc0->sighandle[signum] = handler;

    return 0;
}

int32_t sigreturn(void) {
    int32_t retval;
    asm volatile("                          \n\
                 movl   52(%%ebp), %%edx    \n\
                 leal   4(%%edx), %%ecx     \n\
                 leal   8(%%ebp), %%eax     \n\
                 pushl  $0x34               \n\
                 pushl  %%ecx               \n\
                 pushl  %%eax               \n\
                 call   memcpy              \n\
                 movl   32(%%ebp), %0       \n\
                 "
                 : "=r" (retval)
                );

    return retval;
}
