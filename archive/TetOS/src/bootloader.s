
[BITS 16]
[ORG 0X7C00]

_start:
	; i fucking hate qemu. it somehow turns DL into 32085 so im just hardcoding it to 0x80
	mov	[BOOT_DRIVE], edx ; storing the boot drive number
	mov	[si], edx
	call	itos
	mov	si, di
	call	printf

	mov	si, STARTUP_MSG ; loading the startup message
	call	printf		; printing the message

	call	debug		; remove this

	mov	ax, 0x0
	mov	es, ax

	mov	bx, 0x1000	; destination offset
	mov	al, 0x1		; number of sectors
	mov	ch, 0x0		; cylinder
	mov	cl, 0x2		; sector
	mov	dh, 0x0		; head
	
	;mov	dl, 0x80	; drive

	mov	dl, 0x80

	mov	ah, 0x02	; disk mode
	int	0x13		; interrupting bios to read
	jc	disk_error	; disk error (jump if carry flag)

	mov	si, LOADING_MSG
	call	printf

	;call	DISK_TEST

	jmp	0x0:0x1000		; starting fat16

	mov	si, UNEXPECTED_MSG
	call	printf

; SI - String
printf:
	lodsb		 ; next byte from SI to AL
	or	al, al 	 ; is AL 0?
	JZ	.done	 ; if it is, we're done.
	mov	ah, 0x0E ; BIOS interrupt code
	int 	0x10	 ; interrupting bios
	jmp	printf	 ; repeating this process.
.done:
	ret		 ; done

disk_error:
	mov	si, DISK_ERR_MSG
	call	printf

	mov	si, ERR_CODE_MSG
	call	printf

	mov	al, ah
	xor	ah, ah
	mov	si, ax
	call	itos
	mov	si, di
	call	printf
	mov	si, NEWLINE_MSG
	call	printf
	ret
	; jmp	$

debug:
	mov	si, DISK_NUMBER
	call	printf

	mov	si, 0x80
	call	itos
	mov	si, di
	call	printf
	mov	si, NEWLINE_MSG
	call	printf

	ret
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


; DO NOT USE THIS.
; if this makes this out of the testing, count me dead.
DISK_TEST:
	; the disk is at 0x0:0x1000, so the string is there too
	mov	ax, 0x0
	mov	ds, ax
	mov	si, 0x1000
	call	printf
	;mov	ah, 0x0E
	;mov	al, [0x1000]
	;int	0x10

	jmp	$
; Bootloader messages.
	
STARTUP_MSG	db	"Bootloader: Welcome to TetOS.", 0x0D, 0x0A, 0
LOADING_MSG	db	"Bootloader: Initializing FAT16...", 0x0D, 0x0A, 0
DISK_ERR_MSG	db	"Bootloader: Disk err.", 0x0D, 0x0A, 0
DISK_NUMBER	db	"Bootloader: DISK:", 0
AH_MSG		db	" ", 0xD, 0xA, 0
ERR_CODE_MSG	db	"ERROR CODE: ", 0
UNEXPECTED_MSG	db	"Bootloader: If you see this, the bootloader failed.", 0x0D, 0x0A, 0
NEWLINE_MSG	db	0x0D, 0x0A, 0

; Misc

BOOT_DRIVE	db	0x0

times		510 - ($ - $$) db 0
dw		0xAA55
