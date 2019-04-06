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

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
    uint8_t century;
} time_t;

void init_rtc(void);

void handle_rtc(void);

#endif /* RTC_H */