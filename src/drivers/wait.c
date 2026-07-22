
#include "wait.h"
#include "cmos/cmos.h"
#include "../kernel/idt/idt.h"
#include "../terminal/terminal.h"
#include <stdint.h>

extern volatile uint32_t ticks;

static int seconds_of_day(CMOS_T t) {
    return t.hours * 3600 + t.minutes * 60 + t.seconds;
}

void wait(int seconds) {
    int start = seconds_of_day(read_rtc());

    while (1) {
        int now = seconds_of_day(read_rtc());

        int elapsed = now-start;
        if (elapsed<0){
            elapsed+=24*3600;
        }
        if (elapsed>=seconds){
            break;
        }
    }
}

void sleep(int ms) {
    uint32_t start = ticks;
    
    while ((ticks - start) < ms) {
        debug("TIMER: ", ticks, terminal_color);
    }
}