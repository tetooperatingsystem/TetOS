bits 32
org 0x0

;; Terminal column is stored at 0x1000
;; Terminal row is stored at 0x1002
;; both are 2 bytes

_start:
    mov     edi, 0xB8000    ;; VGA text memory
    
    mov     ax, [0x1000]
    mov     [COLUMN], ax
    mov     ax, [0x1002]
    mov     [ROW], ax

    mov     esi, msg        
    cld

    mov     ax, 80
    mov     bx, [ROW]

    mul     bx

    add     ax, [COLUMN]

    ;; Add twice, because an entry is 16 bit

    movzx   eax, ax

    add     edi, eax
    add     edi, eax

    ;; Add offset

    mov     bh, 1
    call    print

    

    ret

print:
    lodsb
    cmp     al, 0
    je      .done

    ;; High nibble - BG
    ;; Low  nibble - FG

    mov     bl, 0x0     ;; BG
    add     bh, 0x1     ;; FG

    cmp     bh, 0xF
    jg      .bh_f
.print2:
    shl     bl, 0x4     ;; Move bg into BH
    or      bl, bh      ;; Combine

    mov     ah, bl

    mov     [edi], ax

    add     edi, 0x2
    jmp     print
.done:
    ret
.bh_f:
    mov     bh, 0x1
    jmp     .print2

msg db "FORTNITE BALLS IM GAY I LIKE BOYS", 0x0
COLUMN dw 0x0
ROW dw 0x0

