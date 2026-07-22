
#ifndef SOUND_C
#define SOUND_C


#include "sound.h"
#include "../../kernel/ports.h"
#include <stdint.h>


void PlaySound(uint32_t Frequency) {
    uint32_t Div;
    uint32_t Tmp;

    // Set PIT to frequency
    if (Frequency==0) return;
    Div = 1193180 / Frequency; 
    outb(0x43, 0xb6);
    outb(0x42, (uint8_t) (Div));
    outb(0x42, (uint8_t) (Div >> 8));

    // Play sound via PC speakers
    Tmp = inb(0x61);

    if (Tmp != (Tmp | 3)) {
        outb(0x61, Tmp | 3);
    }
}



void MuteSound() {
    uint8_t tmp = inb(0x61) & 0xFC;
    outb(0x61, tmp);
}



void Beep() {
    PlaySound(1000);
    for (int i = 0; i < 192456; i++) {}
    MuteSound();
}

#endif
