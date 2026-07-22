
bits 32

extern isr0,isr1,isr2,isr3,isr4,isr5,isr6,isr7,isr8,isr9,isr10,isr11,isr12,isr13,isr14,isr15,isr16,isr17,isr18,isr19,isr20,isr21,isr22,isr23,isr24,isr25,isr26,isr27,isr28,isr29,isr30,isr31

global irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7, irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15, irq16, irq17,irq18,irq19,irq20,irq21,irq22,irq23,irq24,irq25,irq26,irq27,irq28,irq29,irq30,irq31
irq0:
    pusha
    call    isr0
    popa
    iretd

irq1:
    pusha
    call    isr1
    popa
    iretd

irq2:
    pusha
    call    isr2
    popa
    iretd

irq3:
    pusha
    call    isr3
    popa
    iretd
    
irq4:
    pusha
    call    isr4
    popa
    iretd

irq5:
    pusha
    call    isr5
    popa
    iretd

irq6:
    pusha
    call    isr6
    popa
    iretd

irq7:
    pusha
    call    isr7
    popa
    iretd

irq8:
    pusha
    call    isr8
    popa
    iretd

irq9:
    pusha
    call    isr9
    popa
    iretd

irq10:
    pusha
    call    isr10
    popa
    iretd

irq11:
    pusha
    call    isr11
    popa
    iretd
    
irq12:
    pusha
    call    isr12
    popa
    iretd

irq13:
    pusha
    call    isr13
    popa
    iretd

irq14:
    pusha
    call    isr14
    popa
    iretd

irq15:
    pusha
    call    isr15
    popa
    iretd

irq16:
    pusha
    call    isr16
    popa
    iretd

irq17:
    pusha
    call    isr17
    popa
    iretd

irq18:
    pusha
    call    isr18
    popa
    iretd

irq19:
    pusha
    call    isr19
    popa
    iretd
    
irq20:
    pusha
    call    isr20
    popa
    iretd

irq21:
    pusha
    call    isr21
    popa
    iretd

irq22:
    pusha
    call    isr22
    popa
    iretd

irq23:
    pusha
    call    isr23
    popa
    iretd

irq24:
    pusha
    call    isr24
    popa
    iretd

irq25:
    pusha
    call    isr25
    popa
    iretd

irq26:
    pusha
    call    isr26
    popa
    iretd

irq27:
    pusha
    call    isr27
    popa
    iretd
    
irq28:
    pusha
    call    isr28
    popa
    iretd

irq29:
    pusha
    call    isr29
    popa
    iretd

irq30:
    pusha
    call    isr30
    popa
    iretd

irq31:
    pusha
    call    isr31
    popa
    iretd