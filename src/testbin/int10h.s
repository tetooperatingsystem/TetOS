bits 32
org 0x0

_start:
	mov	si, msg
	call	print

	ret

;; SI = pointer

print:
	lodsb
	mov	ah, 0xE
	cmp	al, 0x0
	je	.done
	mov	bl, 0xDE ;; Light magenta bg with Yellow fg
	int	0x10
	jmp	print
.done:
	ret

msg db "HELLO WITH ISR(AEL)", 0xA, 0xD, "+7k", 0x0
