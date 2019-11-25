/*!
 * tty.h
 * @ Defines and function definitions for tty and line discipline management
 */

#ifndef TTY_H
#define TTY_H

#include "types.h"

typedef struct {
    int32_t ctrl;
    int32_t shift;
    int32_t capslock;
} modifiers_t;

#define LINE_MAX    256

typedef struct {
    int8_t buffer[LINE_MAX];
    int32_t index;
} lbuf_t;

typedef struct {
    uint32_t status;
    int32_t pid;
    int32_t nproc;
    lbuf_t line;
} tty_t;

#define TTY_IDLE    0
#define TTY_ACTIVE  1
#define TTY_READ    2

#define TTY_MAX     4

extern tty_t* tty0;

void init_tty(void);

void start_tty(int32_t index);

void handle_event(uint32_t scancode);

/* File operations (tty) */

int32_t tty_read(int32_t fd, int8_t* buf, int32_t nbytes);
int32_t tty_write(const int8_t* buf, int32_t nbytes);
int32_t tty_open(void);
int32_t tty_close(void);

#endif /* TTY_H */
