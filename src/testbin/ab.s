
bits 32

_start:
	mov	edi, 0xB8000

	mov	byte [edi], 'N'
	mov	byte [edi + 1], 0xF

	ret

