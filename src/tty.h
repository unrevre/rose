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

#define TTY_MAX     4

extern tty_t* tty0;

void init_tty(void);

void start_tty(tty_t* tty);
void clear_tty(tty_t* tty);

void handle_event(uint32_t scancode);

#endif /* TTY_H */
