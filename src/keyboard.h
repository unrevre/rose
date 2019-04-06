/*!
 * keyboard.h
 * @ Defines and function definitions for handling keyboard input
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KBD_DATA_PORT           0x60
#define KBD_COMMAND_PORT        0x64

#define KBD_CONTINUE            0x30

void handle_kbd(void);

#endif /* KEYBOARD_H */
