#ifndef CMOS_C
#define CMOS_C

#include "cmos.h"
#include "../../terminal/terminal.h"
#include "../../kernel/ports.h"
#include "../../mem.h"

uint8_t cmos_read(uint8_t reg) {
    outb(0x70, reg);
    return inb(0x71);
}

int get_update_in_progress_flag() {
    outb(0x70, 0xA);
    return (inb(0x71) & 0x80);
}

int8_t timezone;

CMOS_T read_rtc(void)
{
    CMOS_T last;
    CMOS_T cur;

    do {
        while (get_update_in_progress_flag());

        last.seconds      = cmos_read(CMOS_REG_SECONDS);
        last.minutes      = cmos_read(CMOS_REG_MINUTES);
        last.hours        = cmos_read(CMOS_REG_HOURS);
        last.weekday      = cmos_read(CMOS_REG_WEEKDAY);
        last.day_of_month = cmos_read(CMOS_REG_DAY_OF_MONTH);
        last.month        = cmos_read(CMOS_REG_MONTH);
        last.year         = cmos_read(CMOS_REG_YEAR);

        while (get_update_in_progress_flag());

        cur.seconds       = cmos_read(CMOS_REG_SECONDS);
        cur.minutes       = cmos_read(CMOS_REG_MINUTES);
        cur.hours         = cmos_read(CMOS_REG_HOURS);
        cur.weekday       = cmos_read(CMOS_REG_WEEKDAY);
        cur.day_of_month  = cmos_read(CMOS_REG_DAY_OF_MONTH);
        cur.month         = cmos_read(CMOS_REG_MONTH);
        cur.year          = cmos_read(CMOS_REG_YEAR);

    } while (memcmp(&last, &cur, sizeof(CMOS_T)) != 0);

    cur.hours += GetTimezone();

    cur.seconds = bcd_to_dec(cur.seconds);
    cur.minutes = bcd_to_dec(cur.minutes);
    cur.hours   = bcd_to_dec(cur.hours);
    cur.weekday = bcd_to_dec(cur.weekday);
    cur.day_of_month = bcd_to_dec(cur.day_of_month);
    cur.month = bcd_to_dec(cur.month);
    cur.year = bcd_to_dec(cur.year); 

    return cur;
}

int8_t GetTimezone() {
    return timezone;
}

void SetTimezone(int8_t val) {
    timezone = val;
}

int bcd_to_dec(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0xF);
}

#endif