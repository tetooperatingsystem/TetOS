#include "vga.h"
#include "../../kernel/ports.h"
#include "../../terminal/terminal.h"

#include <stdint.h>
#include <stdbool.h>

bool VGA_ON = false;

void vga_init() {
    printf("VGA Initialized!\n", terminal_color);
    VGA_ON = (bool) (*(uint8_t*) 0x9990);
    // ENABLE DISPLAY
    inb(0x3DA);
    outb(0x3C0, 0x20);
}
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

void vga_set_pixel(size_t x, size_t y, uint8_t color) {
    if (x >= VGA_GRAPHICS_WIDTH || y >= VGA_GRAPHICS_HEIGHT)
        return;
    volatile uint8_t* buff = (volatile uint8_t*) VGA_GRAPHICS;
    size_t index = y * VGA_GRAPHICS_WIDTH + x;
    buff[index] = color;
}

void fill(uint8_t color) {
    volatile uint8_t* buff = (volatile uint8_t*) VGA_GRAPHICS;
    for (size_t i = 0; i < VGA_GRAPHICS_WIDTH * VGA_GRAPHICS_HEIGHT; i++) {
        buff[i] = color;
    }
}

void move_tcursor(int x, int y) {
    uint16_t pos = y * VGA_TEXT_WIDTH + x;

    outb(0x3D4, 0xF);
    outb(0x3d5, (uint8_t) (pos & 0xFF));
    outb(0x3d4, 0xE);
    outb(0x3d5, (uint8_t) ((pos >> 8) & 0xFF));
}

bool is_vgaon() { return VGA_ON; }