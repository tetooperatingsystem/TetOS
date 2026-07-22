bits 32

section .text

_start:
    mov     edi, 0xB8000    ;; VGA text memory
    mov     esi, msg        
    cld
    call    print

    ret

print:
    lodsb
    cmp     al, 0
    je      .done

    mov     ah, 0x4
    mov     [edi], ax

    add     edi, 0x2
    jmp     print
.done:
    ret

section .data

msg db "HELLO WORLD", 0x0
