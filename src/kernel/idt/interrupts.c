
#ifndef IDT_INT
#define IDT_INT

#include "../../terminal/terminal.h"
#include <stdint.h>

#define IRQ_START 32

/*
This file is actually for IRQ, im too lazy to name it accordingly
*/

volatile uint32_t ticks;

void send_eoi(int irq) {
    if (irq >= 8) outb(0xA0, 0x20);
    outb(0x20, 0x20);
}

void isr0(void) {
    //ticks++;
    send_eoi(0);
}

void isr1() {
    inb(0x60);
    send_eoi(1);
}

void isr2() {
    send_eoi(2);
}

void isr3() {
    send_eoi(3);
}

void isr4() {
    send_eoi(4);
}

void isr5() {
    send_eoi(5);
}

void isr6() {
    send_eoi(6);
}

void isr7() {
    send_eoi(7);
}

void isr8() {
    send_eoi(8);
}

void isr9() {
    send_eoi(9);
}



void isr10() {
   send_eoi(10);
}

void isr11() {
    send_eoi(11);
}

void isr12() {
    send_eoi(12);
}

void isr13() {
    send_eoi(13);
}  

void isr14() {
    send_eoi(14);
}

void isr15() {
    send_eoi(15);
}

void isr16() {
    send_eoi(16);
}

void isr17() {
    send_eoi(17);
}

void isr18() {
    send_eoi(18);
}

void isr19() {
    send_eoi(19);
}

void isr20() {
    send_eoi(20);
}

void isr21() {
    send_eoi(21);
}

void isr22() {
    send_eoi(22);
}

void isr23() {
    send_eoi(23);
}

void isr24() {
    send_eoi(24);
}

void isr25() {
    send_eoi(25);
}

void isr26() {
    send_eoi(26);
}

void isr27() {
    send_eoi(27);
}

void isr28() {
    send_eoi(28);
}

void isr29() {
    send_eoi(29);
}

void isr30() {
    send_eoi(30);
}

void isr31() {
    send_eoi(31);
}

#endif