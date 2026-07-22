#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../terminal/terminal.h"
#include "../drivers/vga/vga.h"
#include "../drivers/ps2/keyboard/kb_ps2.h"
#include "../drivers/fat16/fat16.h"
#include "../drivers/vesa/vesa.h"
#include "../drivers/fonts/psf.h"
#include "../drivers/sound/sound.h"
#include "idt/idt.h"
#include "ports.h"
#include "drive_io.h"
#include "../mem.h"

#include "../shell/shell.h"

#if defined(__linux__)
#error "NOT A CROSS COMPILER"
#endif

extern uint16_t ps2_kb_count;
extern char* ps2_kb_BUFF;

extern uint8_t fg;
extern uint8_t bg;

size_t strlen2(const char* str) {
	size_t len = 0;
	while (str[len]) len++;
	return len;
}

int stoi2(char* str) {
    int len = strlen(str);
    int num = 0;

    for (int i = 0; str[i] != 0; i++) {
        char ch = str[i];
        if (ch < '0'  || ch > '9') continue;
        num = num * 10 + (ch - '0');
    }

    return num;
}

void kernel_main( void ) {
	if (IsVesaOn()) {
		VesaInit();
	}
	
	// Terminal shiz

	term_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
	fat16_init(ATA_SLAVE);

	PSF_Init("Sys/Fonts/Font1.PSF");

	for (int y = 0; y < GetHeight(); y++) {
		for (int x = 0; x < GetWidth(); x++) {
			term_putentryat(' ', terminal_color, x, y);
		}
	}
	term_init();
	ps2_kb_init();
	vga_init();
	init_idt();

	printf("  WELCOME TO TETOS!  \n", vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));

	printf("Executing AUTOEXEC.TTO...\n", terminal_color);

	char autoexec[8192];
	read_file(autoexec, "AUTOEXEC.TTO", ATA_SLAVE);

	size_t len = strlen(autoexec);

	char line[100];
	int line_ind = 0;

	for (int i = 0; i < len; i++) {
		if (autoexec[i] == '\n') {
			line[line_ind] = 0;
			shell_exec(line);
			memset(line, 0, 100);
			line_ind = 0;
		}
		else line[line_ind++] = autoexec[i];
	}
	if (*line) shell_exec(line);

	//asm volatile ("sti");
	shell_main();
	
	for (;;) {
		__asm__ volatile ("hlt");
	}
}
