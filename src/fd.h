/*!
 * fd.h
 * @ Defines and function definitions for file descriptors
 */

#ifndef FD_H
#define FD_H

#include "types.h"

typedef struct {
    int32_t (*read)(int32_t, int8_t*, int32_t);
    int32_t (*write)(const int8_t*, int32_t);
    int32_t (*open)(void);
    int32_t (*close)(void);
} fops_t;

extern fops_t tty_fops;

typedef struct {
    uint32_t inode;
    uint32_t flags;
    uint32_t fpos;
    fops_t* fops;
} fd_t;

#define FD_CLOSE    0x0
#define FD_OPEN     0x1
#define FD_READ     0x2
#define FD_WRITE    0x4

extern fd_t stdin;
extern fd_t stdout;

void init_fd(void);

#endif /* FD_H */
