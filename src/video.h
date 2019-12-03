/*!
 * video.h
 * @ Includes for video display
 */

#include "tty.h"
#include "types.h"

void print(tty_t* tty, uint8_t c);
void backspace(tty_t* tty);
void display(tty_t* tty, uint8_t c);
void newline(tty_t* tty);
void scroll(tty_t* tty);
void clear(tty_t* tty);
void blink(void);

void vputc(uint8_t c);
