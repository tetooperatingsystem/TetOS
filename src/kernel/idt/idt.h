#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDT_ADDRESS 0x0010f000

// Interrupt Descriptor (32-bit)
typedef struct __attribute__((packed)) InterruptDESC32 {
    uint16_t offset_1;      // Offset bits 0..15
    uint16_t selector;     // A code segment in either GDT or LDT
    uint8_t  zero;         // Unused
    uint8_t  type_attr;    // Gate type, DPL and P fields
    uint16_t offset_2;      // Offset bits 16..31
} interrupt_descriptor_32_t;

// Interrupt Descriptor Table Register
typedef struct {
    uint16_t limit;        // IDT size - 1
    uint32_t base;         // IDT Address
} __attribute__((packed)) idtr_t;

void init_idt();

void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

// ISR entries functions
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

#endif