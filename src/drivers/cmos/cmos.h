#ifndef CMOS_H
#define CMOS_H

#include "../../mem.h"
#include <stdint.h>

/*
    Register    Contents                Range
    0x00         Seconds                0-59
    0x02         Minutes                0-59
    0x04         Hours                  0-23 in 24-hour mode, 1-12 in 12-hour mode, highest bit set if PM
    0x06         Weekday                1-7, Sunday=1
    0x07         Day of Month           1-31
    0x8          Month                  1-12
    0x9          Year                   0-99
    0x32         Century(maybe)         19-20?
    0x0A         Status Register A
    0x0B         Status Register B
*/

#define CMOS_REG_SECONDS 0x0
#define CMOS_REG_MINUTES 0x2
#define CMOS_REG_HOURS 0x4
#define CMOS_REG_WEEKDAY 0x6
#define CMOS_REG_DAY_OF_MONTH 0x7
#define CMOS_REG_MONTH 0x8
#define CMOS_REG_YEAR 0x9
#define CMOS_REG_CENTURY 0x32
#define CMOS_REG_STATUS_REG_A 0xA
#define CMOS_REG_STATUS_REG_B 0xB

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t weekday;
    uint8_t day_of_month;
    uint8_t month;
    uint8_t year;
    uint8_t century;
    uint8_t stat_reg_A;
    uint8_t stat_reg_B;
} __attribute__ ((packed)) CMOS_T;

uint8_t cmos_read(uint8_t reg);
CMOS_T read_rtc();

int8_t GetTimezone();

void SetTimezone(int8_t val);

int bcd_to_dec(uint8_t bcd);
#endif