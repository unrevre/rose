/*!
 * video.h
 * @ Includes for video display
 */

#include "tty.h"
#include "types.h"

void print(struct tty_t* tty, uint8_t c);
void backspace(struct tty_t* tty);
void display(struct tty_t* tty, uint8_t c);
void newline(struct tty_t* tty);
void scroll(struct tty_t* tty);
void clear(struct tty_t* tty);
void blink(void);

void vputc(uint8_t c);
