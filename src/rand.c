#ifndef RAND_C
#define RAND_C

#include "rand.h"

int rand(void) {
    next = next * 1103515245 + 12345;
    return (unsigned int) (next / 65536);
}

void srand(uint32_t seed) {
    next = seed;
}

#endif