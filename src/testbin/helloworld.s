bits 32
org 0x0

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

msg db "THE CIA GLOWS IN THE DARK", 0x0
