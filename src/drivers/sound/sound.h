

#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>
#include "../../kernel/ports.h"

void PlaySound(uint32_t Frequency);

void MuteSound();

void Beep();

#endif