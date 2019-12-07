/*!
 * keyboard.h
 * @ Defines and function definitions for handling keyboard input
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

#define KBD_DATA_PORT           0x60
#define KBD_COMMAND_PORT        0x64

#define KBD_CONTINUE            0x30

struct modifiers_t {
    int32_t ctrl;
    int32_t shift;
    int32_t capslock;
};

#define LINE_MAX    256

struct lbuf_t {
    int8_t buffer[LINE_MAX];
    int32_t index;
};

void init_kbd(void);

void handle_kbd(void);
void handle_key(uint32_t scancode);

#endif /* KEYBOARD_H */
