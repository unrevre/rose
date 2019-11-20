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

int32_t rtc_read(int32_t fd, int8_t* buf, int32_t nbytes) {
    return -1;
}

int32_t rtc_write(const int8_t* buf, int32_t nbytes) {
    return -1;
}

int32_t rtc_open(void) {
    return -1;
}

int32_t rtc_close(void) {
    return -1;
}
