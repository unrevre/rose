/*!
 * keyboard.c
 * @ Functions for handling keyboard input
 */

#include "keyboard.h"

#include "i8259.h"
#include "lib.h"
#include "types.h"

void init_kbd(void) {
    /* Empty */
}

void handle_kbd(void) {
    cli();
    send_eoi(IRQ_KBD);

    uint8_t key;
    key = inb(KBD_DATA_PORT);
    (void)key;

    sti();
}
