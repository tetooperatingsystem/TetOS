#ifndef KB_PS2
#define KB_PS2

#pragma once

#include <stdint.h>
#include "../../../kernel/ports.h"
#include <stdbool.h>

#define PS2_KB_BUFF_SIZE 512
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_COMMAND_PORT 0x64
#define KEYBOARD_STATUS_PORT 0x64

#define LEFT_SHIFT_SC 0x2A
#define RIGHT_SHIFT_SC 0x36
#define CAPS_LOCK_SC   0x3A
#define BACKSPACE_SC   0x0E
#define ENTER_SC       0x1C

typedef struct {
    bool ShiftL;
    bool ShiftR;
    bool AltL;
    bool AltR;
    bool CtrlL;
    bool CtrlR;
    bool CapsLock;
} key_state;

typedef struct {
    uint16_t low_offset;
    uint16_t selector;
    uint8_t always0;
    uint8_t flags;
    uint16_t high_offset;
} __attribute__ ((packed)) idt_gate_t;

typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;

    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} ps2kb_registers_t;

typedef uint8_t scancode_t;

char scancode_to_ascii(scancode_t scancode);

void ps2_kb_init();

// Wait for Input

scancode_t ps2_kb_wfi();

scancode_t ps2_kb_input();

#endif