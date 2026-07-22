#include "mem.h"
#include <stddef.h>

void* memset(void* dest, int val, size_t count) {
    unsigned char* ptr = (unsigned char*) dest;

    for (size_t i = 0; i < count; i++) {
        ptr[i] = (unsigned char) val;
    }

    return dest;
}

void* memcpy(void* dest, const void* src, size_t count) {
    unsigned char* ptr = (unsigned char*) dest;
    unsigned char* ptr2 = (unsigned char*) src;

    for (size_t x = 0; x < count; x++) {
        ptr[x] = ptr2[x];
    }

    return dest;
}

int memcmp(const char* a, const char* b, size_t size) {
    for (int i = 0; i < size; i++) {
        if (a[i] != b[i]) return 0;
    }

    return 1;
}