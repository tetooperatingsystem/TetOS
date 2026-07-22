#ifndef VESA_H
#define VESA_H

// From boot.s, we receive Vesa info at 0x8000:0x0
// volatile unsigned char *p = (volatile unsigned char *)0x80000;

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t attributes;

    uint8_t window_a;
    uint8_t window_b;
    uint16_t granularity;
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;
    uint16_t pitch;         // !!! BYTES PER LINE
    uint16_t width;
    uint16_t height;
    uint8_t w_char;
    uint8_t y_char;
    uint8_t planes;
    uint8_t bpp;            // !!! Bits per pixel
    uint8_t banks;
    uint8_t memory_model;
    uint8_t bank_size;
    uint8_t image_pages;
    uint8_t reserved0;
    uint8_t red_mask;
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mark;
    uint8_t reserved_position;
    uint8_t direct_color_attributes;
    uint32_t framebuffer;   // !!! Physical address of the framebuffer.
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;
    uint8_t reserved1[206];
}__attribute__((packed)) VBEModeInfoStruct;

void SetPixel(int x, int y, uint8_t color);
void VesaInit();
void Clear(uint8_t c);
bool IsVesaOn();

#endif