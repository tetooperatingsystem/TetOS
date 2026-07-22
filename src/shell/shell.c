#include "../terminal/terminal.h"
#include "../mem.h"
#include "../kernel/drive_io.h"
#include "../drivers/fat16/fat16.h"
#include "../drivers/vga/vga.h"
#include "../kernel/ports.h"

#include "shell_func.h"

char parsed[32][32];

extern uint8_t F16_DRIVE;

extern char filepath[256];

char cmd[1055];

extern SHELL_CMD cmds[];


uint8_t running = 1;

void str_toupper(char* s) {
    for (int i = 0; s[i]; i++) {
        if (s[i] >= 'a' && s[i] <= 'z')
            s[i] -= 32;
    }
}



void parse() {
    // Cleaning the parsed array
    memset(parsed, 0, sizeof(parsed));

    size_t word = 0;
    size_t char_i = 0;

    for (size_t i = 0; cmd[i] != '\0'; i++) {
        if (cmd[i] == ' ') {
            if (char_i > 0) {
                parsed[word][char_i] = '\0';
                word++;
                char_i = 0;
            }
        }
        else {
            if (word < 32 && char_i < 31) {
                parsed[word][char_i++] = cmd[i];
            }
        }
    }

    if (char_i > 0 && word < 32)
        parsed[word][char_i] = '\0';
}

int term_fg;
int term_bg;

void process() {
    if (parsed[0][0] == 0 || parsed[0][0] == '#')
        return;

    char pars[32];

    memcpy(pars, parsed[0], 32);

    str_toupper(pars);

    for (int i = 0; cmds[i].name != NULL; i++) {
        SHELL_CMD command = cmds[i];

        if (strcmp(command.name, pars)) {
            command.func(parsed);
            return;
        }
    }

    // Try finding it in /Bin/

    char npath[256];

    strappend(npath, "/Bin/");
    strappend(npath, pars);
    strappend(npath, ".BIN");

    if (F16_DRIVE == 0) goto end_pr;

    DirectoryEntry file = find_file(npath);

    if (file.FileAttributes & 0x10) {
        end_pr:

        printf("COULD NOT EXECUTE: ", terminal_color);
        printf(parsed[0], terminal_color);
        return;
    }

    read_file(NULL, npath, ATA_SLAVE);

    void (*entry)() = (void(*)()) NULL;

    entry();

    memset(NULL, 0, 0x1000);
    memset(npath, 0, 256);
}

void shell_exec(const char* msg) {
    memcpy(cmd, msg, 1055);

    parse();
    process();
}

void shell_main() {
    printf("SHELL initialized!", terminal_color);

    memset(filepath, 0, 256);

    filepath[0] = '/';

    while (true) {
        printf("\n> ", vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_YELLOW));
        input_s(cmd, 527,  '\n', vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_YELLOW));
        putchar('\n', terminal_color);
        parse();
        process();
    }
}