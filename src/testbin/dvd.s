bits 32
org 0x0

section .text

_start:
    call    clear
    call    wait_for_input

    ret

clear:
    mov   edi, [VGA_MEM]
    mov   ah, [BG]
    mov   al, ' '
    mov   ecx, 2000
.loop:
    mov   [edi], ax
    add   edi, 2
    loop  .loop

    ret
.done:
    ret

wait_for_input:
    in      al, 0x64 ;; 0x64 - Keyboard Status Port

    test    al, 1    ;; If Status(AL)'s bit 0 is 0, keep waiting
    jz      wait_for_input

    in      al, 0x60 ;; Drain buffer
    ret

;; DX - X, CX - Y
putentryat:
    mov     ESI, [VGA_MEM]
    
    ;; Position
    
    mov     AX, CX
    mov     BX, 80
    mul     bx
    movzx   EAX, AX
    add     ESI, EAX
    add     ESI, EAX
    movzx   EAX, DX
    add     ESI, EAX
    add     ESI, EAX

    ;; Color

    mov     bl, [BG]
    mov     bh, [FG]

    shl     bl, 0x4     ;; Move BG into BH
    or      bl, bh      ;; Combine

    ;; Character is in CHAR

    mov     al, [CHAR]

    ;; Print

    mov     ah, bl
    mov     [edi], ax

    ret


section .data

VGA_MEM dd 0xB8000
BG db 0x1
FG db 0xF
CHAR db 'A'