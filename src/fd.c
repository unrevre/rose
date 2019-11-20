/*!
 * fd.c
 * @ Functions for file descriptors
 */

#include "fd.h"

#include "fsdef.h"
#include "rtc.h"
#include "tty.h"

fops_t rtc_fops;
fops_t tty_fops;
fops_t dir_fops;
fops_t file_fops;

fd_t stdin;
fd_t stdout;

void init_fd(void) {
    rtc_fops.read = &rtc_read;
    rtc_fops.write = &rtc_write;
    rtc_fops.open = &rtc_open;
    rtc_fops.close = &rtc_close;

    tty_fops.read = &tty_read;
    tty_fops.write = &tty_write;
    tty_fops.open = &tty_open;
    tty_fops.close = &tty_close;

    dir_fops.read = &dir_read;
    dir_fops.write = &dir_write;
    dir_fops.open = &dir_open;
    dir_fops.close = &dir_close;

    file_fops.read = &file_read;
    file_fops.write = &file_write;
    file_fops.open = &file_open;
    file_fops.close = &file_close;

    stdin.inode = 0;
    stdin.flags = FD_OPEN | FD_READ;
    stdin.fpos = 0;
    stdin.fops = &tty_fops;

    stdout.inode = 0;
    stdout.flags = FD_OPEN | FD_WRITE;
    stdout.fpos = 0;
    stdout.fops = &tty_fops;
}
