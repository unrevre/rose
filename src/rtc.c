/*!
 * rtc.c
 * @ Functions for interacting with the RTC
 */

#include "rtc.h"

#include "i8259.h"
#include "lib.h"

volatile uint32_t rtc_flag;

void init_rtc(void) {
    uint8_t prev;
    outb(RTC_SREG_B, RTC_SREG_PORT);
    prev = inb(RTC_DATA_PORT);
    outb(RTC_SREG_B, RTC_SREG_PORT);
    outb(prev | 0x40, RTC_DATA_PORT);

    rtc_flag = RTC_OPEN;
}

void handle_rtc(void) {
    cli();
    send_eoi(IRQ_RTC);

    outb(RTC_SREG_C, RTC_SREG_PORT);
    inb(RTC_DATA_PORT);

    rtc_flag &= ~RTC_WAIT;

    sti();
}

int32_t set_rtc_frequency(uint32_t frequency) {
    if (frequency & (frequency - 1))
        return -1;

    /* Allowed frequency range: 2 - 1024 Hz */
    if (frequency & 0xFFFFF801)
        return -1;

    uint32_t rate = 0x6;
    while (frequency << rate != 0x10000)
        ++rate;

    cli();

    uint8_t prev;
    outb(RTC_SREG_A, RTC_SREG_PORT);
    prev = inb(RTC_DATA_PORT);
    outb(RTC_SREG_A, RTC_SREG_PORT);
    outb((prev & 0xF0) | rate, RTC_DATA_PORT);

    sti();

    return 0;
}

int32_t rtc_read(int32_t fd, int8_t* buf, int32_t nbytes) {
    if (!(rtc_flag & RTC_OPEN))
        return -1;

    rtc_flag |= RTC_WAIT;
    while (rtc_flag & RTC_WAIT);

    return 0;
}

int32_t rtc_write(const int8_t* buf, int32_t nbytes) {
    if (!(rtc_flag & RTC_OPEN))
        return -1;

    return set_rtc_frequency(*(uint32_t*)buf);
}

int32_t rtc_open(void) {
    if (!(rtc_flag & RTC_OPEN))
        return -1;

    return 0;
}

int32_t rtc_close(void) {
    if (!(rtc_flag & RTC_OPEN))
        return -1;

    return 0;
}
