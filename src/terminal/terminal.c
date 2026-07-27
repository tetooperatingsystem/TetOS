//
// A small file for the terminal functions.
//

#include "../drivers/vga/vga.h"
#include "../drivers/fonts/psf.h"
#include "../drivers/vesa/vesa.h"
#include <stdint.h>
#include "terminal.h"

uint16_t terminal_row;
uint16_t terminal_column;
uint8_t terminal_color;

#define MAX_ROWS 30
#define MAX_COLUMNS 80

uint16_t terminal_buff[MAX_COLUMNS * MAX_ROWS];
uint16_t* terminal_buffer = (uint16_t*) VGA_TEXT;

uint8_t fg;
uint8_t bg;

extern int PSF_VERS;
extern PSF1_Header Info1;
extern PSF_Font Info;

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len]) len++;
	return len;
}

void update_rc() {
	*(volatile uint16_t *) 0x1000 = terminal_column;
	*(volatile uint16_t *) 0x1002 = terminal_row;
}

int GetWidth() {
	if (IsVesaOn()) {
		if (GetPSFVers() == 1) { return 640 / 8; }
		else {
			return 640 / Info.width;
		}
	}
	else {
		return VGA_TEXT_WIDTH;
	}
	return 0;
}

int GetHeight() {
	if (IsVesaOn()) {
		if (GetPSFVers() == 1) { return 480 / Info1.characterSize; }
		else { return 480 / Info.height; }
	}
	else { return VGA_TEXT_HEIGHT; }
	return 0;
}

void draw_term() {
	int width, height;
	width = GetWidth();
	height = GetHeight();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			uint16_t entr = terminal_buff[(y*width)+x];

			char ch = entr & 0xFF;
			uint8_t color = entr >> 8;
			term_putentryat(ch, color,x,y);
		}
	}
}

void scroll(void) {
	int width, height;
	width = GetWidth();
	height = GetHeight();
	size_t count = (height - 1) * width;

	for (size_t i = 0; i < count; i++) {
		terminal_buff[i] = terminal_buff[i + width];
	}

	for (size_t i = count; i < height * width; i++) {
		terminal_buff[i] = vga_entry(' ', terminal_color);
	}

	draw_term();

	update_rc();
}

void term_putentryat(char c, uint8_t color, size_t x, size_t y) {
	int dwasdws = GetWidth();
	if (IsVesaOn()) {
		uint8_t fgaaa = color & 0xF;
		uint8_t bgaaa = color >> 4;
		
		int width, height;
		if (GetPSFVers() == 1) {
			width = 8;
			height = Info1.characterSize;
		}
		else {
			width = Info.width;
			height = Info.height;
		}
		for (int x1 = 0; x1 < width; x1++) {
			for (int y1 = 0; y1 < height; y1++) {
				SetPixel(x1+(x*width),y1+(y*height),bgaaa);
			}
 		}

		if (GetPSFVers() == 1) {
			DrawGlyph(x*8,y*Info1.characterSize,c,fgaaa);
		}
		else if (GetPSFVers() == 2) {
			DrawGlyph(x*Info.width,y*Info.height,c, fgaaa);
		}
	}
	int index = y * dwasdws + x;
	terminal_buff[index]   = (uint8_t) c | color << 8;
	if (!IsVesaOn()) terminal_buffer[index] = (uint8_t) c | color << 8;
}

void putchar(char c, uint8_t COLOR) {
	int width = GetWidth();
	int height = GetHeight();

    if (c == '\n') {
		terminal_column = 0;
		terminal_row++;
		if (terminal_row >= height) {
			scroll();
			terminal_row--;
		}
    }
	else if (c == '\t') {
		for (int i = 0; i < 4; i++) {
			putchar(' ', COLOR);
		}
	}
	else {
		term_putentryat(c, COLOR, terminal_column, terminal_row);
		terminal_column++;
	}

	if (terminal_column >= width) {
		terminal_column = 0;
		terminal_row++;
	}

	if (terminal_row >= height) {
		scroll();
		terminal_row -= 1;
	}

	move_tcursor(terminal_column, terminal_row);
	update_rc();
}

void write(const char* data, size_t size, uint8_t COLOR) {
    for (size_t i = 0; i < size; i++) {
        putchar(data[i], COLOR);
    }
}

void printf(const char* data, uint8_t COLOR) {
    write(data, strlen(data), COLOR);
}

void term_setcolor(enum vga_color fg_, enum vga_color bg_) {
    terminal_color = vga_entry_color(fg_, bg_);
	fg = fg_;
	bg = bg_;
}

void clear_c(enum vga_color color) {
	uint8_t c = vga_entry_color(color, color);
	for (size_t y = 0; y < VGA_TEXT_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_TEXT_WIDTH; x++) {
			term_putentryat(' ', c, x, y);
		}
	}
}

void clear() {
	for (size_t y = 0; y < VGA_TEXT_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_TEXT_WIDTH; x++)
			term_putentryat(' ', terminal_color, x, y);
	}
}

char* itos_h(int number) {
	if (number == 0) {
		return "0x0";
	}
	static char ret[10];
	char out[8];
	int i = 0;

	while (number > 0  && i < 8) {
		int b = number % 16;
		number /= 16;
		if (b <= 9) {
			out[i] = '0' + b;
		}
		else if (b >= 10 && b <= 15) {
			out[i] = 'A' + (b - 10);
		}
		i++;
	}

	// Reverse the outcome

	// -1 to skip the \0
	ret[0] = '0';
	ret[1] = 'x';

	for (int j = 0; j < i; j++) {
		ret[j + 2] = out[i - j - 1];
	}
	ret[i+2] = 0;

	return ret;
}

char* itos(int number) {
	if (number == 0) return "0";

	char out[17];
	int i = 0;
	while (number > 0 && i < 17) {
		int n = number % 10;
		number /= 10;

		out[i] = '0' + n;
		i++;
	}

	out[i] = 0;

	static char res[17];

	for (int j = 0; j < i; j++)
		res[j] = out[i - j - 1];

	res[i]=0;
	
	return res;
}

void printh(int number, uint8_t color) {
	char* str = itos_h(number);
	printf(str, color);
}

void strcpy(char* str1, char* str2) {
	if (strlen(str1) != strlen(str2)) return;

	size_t i = 0;

	while (str1[i]) {
		str2[i] = str1[i];
		i++;
	}
}

void strappend(char* str1, char* str2) {
	size_t offset = strlen(str1);

	int i = 0;

	while (str2[i]) {
		str1[offset + i] = str2[i];
		i++;
	}

	str1[offset + i] = 0;
}

int strcmp(const char* str1, const char* str2) {
    if (strlen(str1) != strlen(str2)) return 0;
    size_t len = strlen(str1);

    for (int i = 0; i < len; i++) {
        if (str1[i] != str2[i]) return 0;
    }
    return 1;
}

void input_s(char* buffer, size_t buffer_size, char stop_character, uint8_t color) {
	scancode_t ch = 0;
	char ascii = 0;

	size_t buffer_count = 0;

	size_t x = terminal_column;
	size_t y = terminal_row;

	while (true) {
		ch = ps2_kb_wfi();

		if (ch & 0x80) continue;

		ascii = scancode_to_ascii(ch);

		if (ascii == stop_character) {
			break;
		}

		if (ascii == '\b') {
			if (buffer_count > 0) {
				if (x > 0) x--;
				else if ( y > 0) {
					x = VGA_TEXT_WIDTH - 1; y--;
				}
	
				term_putentryat(' ', color, x, y);
				buffer_count--;
	
				terminal_column = x;
				terminal_row = y;
				
				buffer[buffer_count] = 0;

				move_tcursor(x, y);
			}

			continue;
		}

		if (buffer_count < buffer_size - 1 && ascii >= 0x20) {
			buffer[buffer_count] = ascii;
			x++;
			if (x >= VGA_TEXT_WIDTH) {
				if (y < VGA_TEXT_HEIGHT - 1) y++;
				else scroll();
				x = 0;
			}
			putchar(ascii, color);

			y = terminal_row;
			x = terminal_column;
			buffer_count++;
	
		}
		move_tcursor(terminal_column, terminal_row);
	}

	buffer[buffer_count] = '\0';
}

void stradd(char* res, char* str1, char* str2) {
	int pos = 0;

	for (int i = 0; str1[i]; i++) {
		pos++;
		res[pos] = str1[pos-1];
	}

	for (int i = 0; str2[i]; i++) {
		pos++;
		res[pos] = str2[pos-1];
	}

	res[pos] = '\0';
}

void input(char* buffer, size_t buffer_size, uint8_t color) {
	input_s(buffer,buffer_size,'\n', color);
}

void input_l(char* buffer, uint16_t limit, uint8_t color) {
	scancode_t ch = 0;
	char ascii = 0;

	size_t buffer_count = 0;

	size_t x = terminal_column;
	size_t y = terminal_row;

	while (buffer_count < limit - 1) {
		ch = ps2_kb_wfi();

		if (ch & 0x80) continue;

		if (terminal_column > 0) x = terminal_column - 1;
		else x = 0;
		y = terminal_row;

		ascii = scancode_to_ascii(ch);

		if (ascii == '\b') {
			if ( x > 0 && buffer_count > 0) {
				x--;
				term_putentryat(' ', color, x, y);
				buffer_count--;
			}
			else if (y > 0) {
				y--;
				x = VGA_TEXT_WIDTH - 1;	
			}
			terminal_column = x;
			terminal_row = y;
			buffer[buffer_count] = 0;
		}

		if (buffer_count < limit - 1 && (ascii >= 0x20 || ascii == 0xA)) {
			buffer[buffer_count] = ascii;
			x++;
			if (x >= VGA_TEXT_WIDTH) {
				x = 0;
				if (y < VGA_TEXT_HEIGHT - 1) y++;
			}
			terminal_row = y;
			terminal_column = x;
			buffer_count++;
			putchar(ascii, color);
		}
		move_tcursor(terminal_column, terminal_row);
	}

	buffer[buffer_count] = '\0';
}

void debug(char* msg, int value, uint8_t color) {
	printf(msg, color);
	printh(value, color);
	putchar('\n', color);
}

void term_init( void ) {
    terminal_row = 0;
    terminal_column = 0;
    term_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
	printf("TERMINAL initialized!\n", terminal_color);
}

uint8_t term_get_bg_color() {
	return bg;
}

uint8_t term_get_fg_color() {
	return fg;
}