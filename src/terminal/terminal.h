//
// A small file for the terminal functions.
//

#pragma once

#include "../drivers/vga/vga.h"
#include "../drivers/ps2/keyboard/kb_ps2.h"

extern uint16_t terminal_row;
extern uint16_t terminal_column;
extern uint8_t terminal_color;
extern uint16_t* terminal_buffer;

void term_init ( void );
void term_putentryat(char c, uint8_t color, size_t x, size_t y);
void putchar(char c, uint8_t COLOR);
void write(const char* data, size_t size, uint8_t COLOR);
void printf(const char* data, uint8_t COLOR);
void term_setcolor(enum vga_color fg, enum vga_color bg);
void scroll(void);
void clear_c(enum vga_color color);
void clear();
// Integer to string in decimal
char* itos(int number);

int GetWidth();
int GetHeight();

// Integer to string in hexadecimal
char* itos_h(int number);
void printh(int number, uint8_t color);
void printn(int number, uint8_t color);
void input(char* buffer,size_t buffer_size, uint8_t color);
// Input, but the input stops when a certain character is pressed
void input_s(char* buffer, size_t buffer_size, char stop_character, uint8_t color);
// Input, but the input stops when a character limit is reached
void input_l(char* buffer, uint16_t limit, uint8_t color);

void strcpy(char* str1, char* str2);

void strappend(char* str1, char* str2);

void debug(char* msg, int value, uint8_t color);

size_t strlen(const char* str);

int strcmp(const char* str1, const char* str2);

void stradd(char* res, char* str, char* str2);

uint8_t term_get_bg_color();
uint8_t term_get_fg_color();