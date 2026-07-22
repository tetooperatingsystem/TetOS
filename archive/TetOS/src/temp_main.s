
[BITS 16]
[ORG 0x8000]

_start:
	mov	si, STARTUP_MSG
	call	printf

printf:
	lodsb
	or	al, al
	jz	.done
	mov	ah, 0x0e
	int	0x10
	jmp	printf
.done:
	ret

STARTUP_MSG	DB "Launcher: Welcome to TetOS.", 0x0D, 0x0A, 0

times		510 - ($ - $$) db 0
dw		0xaa55
