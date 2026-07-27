#include <stdint.h>
#include <stdbool.h>
#include "kb_ps2.h"
#include "../../../kernel/ports.h"
#include "../../../terminal/terminal.h"


// PS/2 keyboard - irq 1

// Only US qwerty, for now.

key_state KeyState;
char ps2_kb_BUFF[PS2_KB_BUFF_SIZE];
uint16_t ps2_kb_count = 0;

char scan_to_ascii[128] = {
    0, 27, '1', '2', '3','4','5','6','7','8','9','0','-','=', '\b', '\t', // 0X0-0XF
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,               // 0x10-0x1F
    'a','s','d','f','g','h','j','k','l',';','\'','`',0, '\\',             // 0x1C-0x2F
    'z','x','c','v','b','n','m',',','.','/',0,                            // 0x30-0X39
    '*', 0, ' '                                                           // 0x3A-0x3C
    // Everything else is unprintable, so we're gonna leave it here
};
char scan_to_ascii_shift[128] = {
    0,   27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b','\t',
    'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',0,
    'A','S','D','F','G','H','J','K','L',':','"','~',0,'|',
    'Z','X','C','V','B','N','M','<','>','?',0,
    '*',0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Rest is unprintable.
};

char scancode_to_ascii(scancode_t scancode) {
    bool shift = KeyState.ShiftL || KeyState.ShiftR;     // If either left shift or right shift are pressed, shift is on (even both)
    if (KeyState.CapsLock || shift) {
        return scan_to_ascii_shift[(uint8_t) scancode];
    }
    else {
        return scan_to_ascii[(uint8_t) scancode];
    }
}

void ps2_kb_init() {
    ps2_kb_count = 0;
    ps2_kb_BUFF[0] = '\0';
    KeyState = (key_state) {false, false, false, false, false, false, false};

    //printf("PS/2 KEYBOARD initialized!\n", terminal_color);
};

void process_keypress(scancode_t sc) {
    bool released = sc & 0x80;
    sc &= 0x7F;

    switch (sc) {
        case LEFT_SHIFT_SC:
            KeyState.ShiftL = !released;
            break;
        case RIGHT_SHIFT_SC:
            KeyState.ShiftR = !released;
            break;
        case CAPS_LOCK_SC:
            if (!released)
                KeyState.CapsLock = !KeyState.CapsLock;
            break;
        case BACKSPACE_SC:
            if (!released && ps2_kb_count > 0) {
                ps2_kb_count--;
                ps2_kb_BUFF[ps2_kb_count] = 0;
            }
            break;
    }
}

// Wait for Input

scancode_t ps2_kb_wfi() {
    scancode_t scancode;
    while (!(inb(KEYBOARD_STATUS_PORT) & 1)) {
        __asm__ volatile("pause");
    }
    scancode = inb(KEYBOARD_DATA_PORT);

    process_keypress(scancode);

    return scancode;
};