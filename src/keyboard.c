/*!
 * keyboard.c
 * @ Functions for handling keyboard input
 */

#include "keyboard.h"

#include "i8259.h"
#include "lib.h"
#include "tty.h"
#include "types.h"

void handle_kbd(void) {
    cli();
    send_eoi(IRQ_KBD);

    uint8_t scancode;
    if ((scancode = inb(KBD_DATA_PORT)) == KBD_CONTINUE) {
        uint8_t next = inb(KBD_DATA_PORT);
        scancode = (scancode << 8) | next;
    }

    handle_event(scancode);

    sti();
}
