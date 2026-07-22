#pragma once

#include <stdint.h>
#include <stddef.h> 
#include <stdbool.h>

#define VGA_TEXT 0xb8000
#define VGA_GRAPHICS ((uint8_t*) 0xa0000)
#define VGA_TEXT_WIDTH 80
#define VGA_TEXT_HEIGHT 25
#define VGA_GRAPHICS_WIDTH 320
#define VGA_GRAPHICS_HEIGHT 200

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENDA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
	VGA_COLOR_LIGHT_YELLOW = 0x0E,
	VGA_COLOR_YELLOW = 0x2C
};

// !!!
// 0 - Text mode.
// 1 - 13H
void vga_init();

void vga_set_pixel(size_t y, size_t x, uint8_t color);

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);

uint16_t vga_entry(unsigned char uc, uint8_t color);

void fill(uint8_t color);

void move_tcursor(int x, int y);

bool is_vgaon();