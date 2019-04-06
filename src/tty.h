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
    uint32_t index;
} tty_t;

void init_tty(void);

void clear_tty(void);

void handle_event(uint32_t scancode);

#endif /* TTY_H */
