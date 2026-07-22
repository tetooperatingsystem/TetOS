; Do sheet idk
; AAAAAAAAA ICANT BREATHE
;
;
;



[ORG 0x7c00]
[BITS 16]

_start:
	mov ah, 0x0E
	mov al, dl
	add al, '0'
	int 0x10

	mov	dl, 0x80
	mov	[BOOT_DRIVE], dl

	; Resetting disk

	mov	ah, 0x0
	int	0x13
	
	mov	si, DRIVE_DBG
	call	printf
	xor	dh, dh
	mov	si, dx
	call	itos
	mov	si, di
	call	printf
	mov	si, NEWLINE
	call	printf

	mov	si, STARTUP_MSG
	call	printf

	mov	si, READING_MSG
	call	printf

	mov	bx, 0x1000
	mov	es, bx
	xor	bx, bx

	mov	al, 0x4		; sector count (30)
	mov	ch, 0x0		; cylinder
	mov	cl, 0x2		; sector
	mov	dh, 0x0		; head
	mov	dl, 0x80	; drive

	mov	ah, 0x02	; func mode
	int	0x13		; interrupting bios
	jc	disk_error	; disk error

	mov	si, PROTECT_MSG
	call	printf

	mov	si, LAUNCH_MSG
	call	printf
	; enabling protected mode

	;mov	eax, cr0
	;or	eax, 1
	;mov	cr0, eax

	jmp	0x1000

	mov	si, 0x1000
	call	printf

	;  this generally shouldn't happen.

	mov	si, UNEXP_MSG
	call	printf

	jmp	$

; si - integer, di - string, dh - offset
itos:
	mov	bx, 10
	mov	byte [di], 0
	dec	di
.loop:
	xor	dx, dx
	mov	ax, si
	div	bx

	;	DX:AX - dividend
	;	AX - quotient
	;	DX - remainder

	add	dl, '0'
	mov	[di], dl
	dec	di

	mov	si, ax
	test	si, si
	jnz	.loop

	inc	di
	ret

printf:
	lodsb		; load next character from SI into AL
	cmp	al, 0	
	je	.done

	mov	ah, 0x0E
	int	0x10

	jmp	printf
.done:
	ret

disk_error:
	mov	si, ERR_MSG
	call	printf
	
	; ah - error code

	mov	dl, ah
	xor	dh, dh
	mov	si, dx
	call	itos
	mov	si, di
	call	printf
	mov	si, NEWLINE
	call	printf

	ret


STARTUP_MSG	db "[Boot]: Hello, World", 0x0A, 0x0D, 0x0
LAUNCH_MSG	db "[Boot]: Attempting launching...", 0x0A, 0x0D, 0x0
PROTECT_MSG	db "[Boot]: Entering protected mode.", 0xA, 0xD, 0x0
READING_MSG	db "[Boot]: Reading kernel initializer...", 0x0A, 0x0D, 0x0
ERR_MSG		db "[ERROR]: [Boot]: Couldn't read disk: ", 0xA, 0xD, 0x0
UNEXP_MSG	db "You're not supposed to be here.", 0xA, 0xD, 0x0
DRIVE_DBG	db "[Boot]: Drive: ", 0x0
NEWLINE		db 0xA, 0xD, 0x0

BOOT_DRIVE	db 0x0

times		510 - ($ - $$) db 0
dw		0xAA55
