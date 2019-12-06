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

void init_kbd(void);

void handle_kbd(void);
void handle_key(uint32_t scancode);

#endif /* KEYBOARD_H */
