
#ifndef VESA_C
#define VESA_C

#include "vesa.h"
#include "../../mem.h"
#include <stdint.h>
#include <stdbool.h>

VBEModeInfoStruct VesaInf;

void VesaInit() {
    // Pointer 
    // From the bootloader, we get the Mode Info at 0x8000:0x0
    volatile unsigned char* p = (volatile unsigned char*) 0x80000;

    memcpy(&VesaInf, (void*) p, sizeof(VesaInf));
}

bool IsVesaOn() {
    bool a = (bool) (*(uint8_t*) 0x9990);
    return a;
}

void SetPixel(int x, int y, uint8_t color) {
    if (x < 0 || y < 0 || x >= VesaInf.width || y >= VesaInf.height) return;
    volatile unsigned char* framebuffer = (volatile unsigned char*) VesaInf.framebuffer;
    int offset = (VesaInf.pitch*y) + x;
    
    framebuffer[offset] = color;
}

void Clear(uint8_t c)
{
    uint8_t *fb = (uint8_t*)VesaInf.framebuffer;

    for(int y = 0; y < VesaInf.height; y++)
    {
        for(int x = 0; x < VesaInf.width; x++)
        {
            fb[y * VesaInf.pitch + x] = c;
        }
    }
}

#endif