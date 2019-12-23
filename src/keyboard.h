/*!
 * keyboard.h
 * @ Defines and function definitions for handling keyboard input
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "tty.h"
#include "types.h"

#define KBD_DATA_PORT           0x60
#define KBD_COMMAND_PORT        0x64

#define KBD_CONTINUE            0x30

void init_kbd(void);

void handle_kbd(void);
void handle_key(struct tty_t* tty, uint32_t scancode);

#endif /* KEYBOARD_H */
