[BITS 16]
[0RG 0x8000]

_start:
	; entering graphics mode
	mov	al, 0x13
	mov	ah, 0x0

	int	0x10		; entering graphics mode




