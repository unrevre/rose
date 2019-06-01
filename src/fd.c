/*!
 * fd.c
 * @ Functions for file descriptors
 */

#include "fd.h"

#include "tty.h"

fops_t tty_fops;

fd_t stdin;
fd_t stdout;

void init_fd(void) {
    tty_fops.read = &tty_read;
    tty_fops.write = &tty_write;
    tty_fops.open = &tty_open;
    tty_fops.close = &tty_close;

    stdin.inode = 0;
    stdin.flags = FD_OPEN | FD_READ;
    stdin.fpos = 0;
    stdin.fops = &tty_fops;

    stdout.inode = 0;
    stdout.flags = FD_OPEN | FD_WRITE;
    stdout.fpos = 0;
    stdout.fops = &tty_fops;
}
