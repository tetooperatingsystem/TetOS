#ifndef RAND_H
#define RAND_H

#include <stdint.h>


static uint32_t next = 1;

int rand(void);

void srand(uint32_t seed);

#endif