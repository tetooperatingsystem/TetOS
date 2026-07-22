#ifndef IDT_C
#define IDT_C

#include <stdint.h>
#include "idt.h"
#include "../ports.h"
#include "../../mem.h"

extern void irq0(), irq1(), irq2(), irq3(), irq4(), irq5(), irq6(), irq7(), irq8(), irq9(), irq10(), irq11(), irq12(), irq13(), irq14(), irq15(), irq16(), irq17(), irq18(), irq19(), irq20(), irq21(), irq22(), irq23(), irq24(), irq25(), irq26(), irq27(), irq28(), irq29(), irq30(), irq31();

idtr_t idtr;
interrupt_descriptor_32_t idt_entries[256];

void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flag) {
    idt_entries[num].offset_1 = base & 0xFFFF;
    idt_entries[num].offset_2 = (base >> 16) & 0xFFFF;
    idt_entries[num].selector = selector;
    idt_entries[num].type_attr = flag;
}

void init_idt() {
    idtr.limit = sizeof(interrupt_descriptor_32_t) * 256 - 1;
    idtr.base = (uint32_t)&idt_entries;

    memset(&idt_entries, 0, sizeof(interrupt_descriptor_32_t)*256);

    // Remap IRQ

    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x4);
    outb(0xA1, 0x2);
    outb(0x21, 0x1);
    outb(0xA1, 0x1);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    // Set interrupt functions

    idt_set_gate(32, (uint32_t) irq0, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t) irq1, 0x8, 0x8E);
    idt_set_gate(34, (uint32_t) irq2, 0x8, 0x8E);
    idt_set_gate(35, (uint32_t) irq3, 0x8, 0x8E);
    idt_set_gate(36, (uint32_t) irq4, 0x8, 0x8E);
    idt_set_gate(37, (uint32_t) irq5, 0x8, 0x8E);
    idt_set_gate(38, (uint32_t) irq6, 0x8, 0x8E);
    idt_set_gate(39, (uint32_t) irq7, 0x8, 0x8E);
    idt_set_gate(40, (uint32_t) irq8, 0x8, 0x8E);
    idt_set_gate(41, (uint32_t) irq9, 0x8, 0x8E);
    idt_set_gate(42, (uint32_t) irq10, 0x8, 0x8E);
    idt_set_gate(43, (uint32_t) irq11, 0x8, 0x8E);
    idt_set_gate(44, (uint32_t) irq12, 0x8, 0x8E);
    idt_set_gate(45, (uint32_t) irq13, 0x8, 0x8E);
    idt_set_gate(46, (uint32_t) irq14, 0x8, 0x8E);
    idt_set_gate(47, (uint32_t) irq15, 8, 0x8E);
    idt_set_gate(48, (uint32_t) irq16, 8, 0x8E);
    idt_set_gate(49, (uint32_t) irq17, 8, 0x8E);
    idt_set_gate(50, (uint32_t) irq18, 8, 0x8E);
    idt_set_gate(51, (uint32_t) irq19, 8, 0x8E);
    idt_set_gate(52, (uint32_t) irq20, 8, 0x8E);
    idt_set_gate(53, (uint32_t) irq21, 8, 0x8E);
    idt_set_gate(54, (uint32_t) irq22, 8, 0x8E);
    idt_set_gate(55, (uint32_t) irq23, 8, 0x8E);
    idt_set_gate(56, (uint32_t) irq24, 8, 0x8E);
    idt_set_gate(57, (uint32_t) irq25, 8, 0x8E);
    idt_set_gate(58, (uint32_t) irq26, 8, 0x8E);
    idt_set_gate(59, (uint32_t) irq27, 8, 0x8E);
    idt_set_gate(60, (uint32_t) irq28, 8, 0x8E);
    idt_set_gate(61, (uint32_t) irq29, 8, 0x8E);
    idt_set_gate(62, (uint32_t) irq30, 8, 0x8E);
    idt_set_gate(63, (uint32_t) irq31, 8, 0x8E);

    __asm__ volatile("lidt %0" : : "m"(idtr));

    //outb(0x21,0xFE);
    //outb(0xA1,0xFE);

    ///asm volatile ("sti");
}

#endif