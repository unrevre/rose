/*!
 * fd.h
 * @ Defines and function definitions for file descriptors
 */

#ifndef FD_H
#define FD_H

#include "types.h"

struct fops_t {
    int32_t (*read)(int32_t, int8_t*, int32_t);
    int32_t (*write)(const int8_t*, int32_t);
    int32_t (*open)(void);
    int32_t (*close)(void);
};

extern struct fops_t rtc_fops;
extern struct fops_t tty_fops;
extern struct fops_t dir_fops;
extern struct fops_t file_fops;

struct fd_t {
    uint32_t inode;
    uint32_t flags;
    uint32_t fpos;
    struct fops_t* fops;
};

#define FD_CLOSE    0x0
#define FD_OPEN     0x1
#define FD_READ     0x2
#define FD_WRITE    0x4

extern struct fd_t stdin;
extern struct fd_t stdout;

void init_fd(void);

#endif /* FD_H */
