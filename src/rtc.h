/*!
 * rtc.h
 * @ Defines and function definitions for interacting with the RTC
 */

#ifndef RTC_H
#define RTC_H

#include "types.h"

#define RTC_SREG_PORT   0x70
#define RTC_DATA_PORT   0x71

#define RTC_SREG_A      0x8A
#define RTC_SREG_B      0x8B
#define RTC_SREG_C      0x8C

#define RTC_IDLE        0
#define RTC_OPEN        1
#define RTC_WAIT        2

void init_rtc(void);

void handle_rtc(void);
int32_t set_rtc_frequency(uint32_t frequency);

int32_t rtc_read(int32_t fd, int8_t* buf, int32_t nbytes);
int32_t rtc_write(const int8_t* buf, int32_t nbytes);
int32_t rtc_open(void);
int32_t rtc_close(void);

#endif /* RTC_H */
