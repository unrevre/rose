/*!
 * tty.h
 * @ Defines and function definitions for tty and line discipline management
 */

#ifndef TTY_H
#define TTY_H

#include "types.h"

#define LINE_MAX    256

struct tty_t {
    uint32_t status;
    int32_t pid;
    int32_t nproc;
    int32_t offset;
    uint8_t* address;
    int32_t index;
    int8_t buffer[LINE_MAX];
};

#define TTY_PEND    0
#define TTY_READ    1

#define TTY_MAX     4

extern struct tty_t* tty0;

void init_tty(void);

void start_tty(int32_t index);
void swap_tty(int32_t index);

uint32_t tty_buffer(struct tty_t* tty);

/* File operations (tty) */

int32_t tty_read(int32_t fd, int8_t* buf, int32_t nbytes);
int32_t tty_write(const int8_t* buf, int32_t nbytes);
int32_t tty_open(void);
int32_t tty_close(void);

#endif /* TTY_H */
