/*!
 * rtc.c
 * @ Functions for interacting with the RTC
 */

#include "rtc.h"

#include "i8259.h"
#include "lib.h"

void init_rtc(void) {
    cli();

    uint8_t prev;
    outb(RTC_SREG_B, RTC_SREG_PORT);
    prev = inb(RTC_DATA_PORT);
    outb(RTC_SREG_B, RTC_SREG_PORT);
    outb(prev | 0x40, RTC_DATA_PORT);

    sti();
}

void handle_rtc(void) {
    cli();
    send_eoi(IRQ_RTC);

    outb(RTC_SREG_C, RTC_SREG_PORT);
    inb(RTC_DATA_PORT);

    sti();
}
