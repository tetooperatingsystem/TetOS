
org 0x7c00
bits 16

_start:

boot_1:
	mov	ax, 0x0
	mov	ds, ax

	mov	[BOOT_DRIVE], dl	;; On boot, DL is the drive we booted from 

	mov	si, INIT_MSG
	call	print			;; Init msg display

	mov		si, PROMPT_MSG
	call	print

disk_prompt:
	call	wfinput			;; Wait for input

process_prompt:
	;; Display the character
	mov	ah, 0xE			
	int	0x10


	cmp	al, '1'			;; This drive
	je	al_1
	cmp	al, '2'			;; Next drive
	je	al_2
	cmp	al, '3'			;; 0x80
	je	al_3
	
	;; If neither of the inputs is defined behaviour, do the prompt again.
	jmp	disk_prompt

al_1:
	mov	dl, [BOOT_DRIVE]	;; DL = BOOT_DRIVE
	jmp	boot_2
al_2:
	mov	dl, [BOOT_DRIVE]	;; DL = BOOT_DRIVE + 1
	add	dl, 0x1
	jmp	boot_2
al_3:
	mov	dl, 0x80		;; DL = 0x80 (128)

boot_2:
graph_prompt:
	mov		si, PROMPT2_MSG		;; prompt the user about graphics
	call	print
	call	wfinput
process_graph_prompt:
	mov		ah, 0xE				;; Display the character
	int		0x10

	cmp		al, '1'				;; Graphics mode
	je		get_vesa_info

	jmp		boot_3				;; Move on

;; es:di - 256-byte buffer
;; cf    - set on error

get_vesa_info:
	clc
	mov		ax, 0x8000
	mov		es, ax
	mov		di, 0x0
	mov		ax, 0x4f01
	mov		cx, 0x101
	int		0x10
	jc		vesa_failed
	cmp		ax, 0x004f
	jne		vesa_failed

go_into_graphics_mode:
	mov		ax, 0x4F02 			;; VESA set video mode
	mov		bx, 0x4101			;; 0x4101 is usually 640x480 8-bit

	int		0x10
	cmp		ax, 0x004f
	jne		vesa_failed

	;; Signaling to the OS that we have graphics
	mov		byte [0x9990], 0x1

	jmp		boot_3

vesa_failed:
	mov		si, GRAPH_MSG
	call	print
	
	stc
	hlt

boot_3:
	mov	si, 0x0
	mov	es, si
	mov	bx, 0x1000		;; Destination = 0x0:0x1000

	mov	al, 0x35		;; SECTOR COUNT
	mov	ch, 0x0			;; CYLINDER
	mov	cl, 0x2			;; SECTOR (STARTS WITH 1)
	mov	dh, 0x0			;; HEAD
	;; Drive already defined

	mov	ah, 0x02 		;; FUNC MODE (READ SECTORS FROM DRIVE)
	int	0x13			;; INTERRUPTING BIOS

	jc	disk_error		;; If 13h carries flag, trigger a disk error


	;; Protected mode

	cli
	lgdt	[gdt_descriptor]

	mov	eax, cr0		;; Enabling it, duh
	or	eax, 1
	mov	cr0, eax

	;; Stack

	mov	esp, 0x900000

	jmp	0x8:protected_mode_exec	;; Executing the kernel, finally


disk_error:
	;; AH - Error code

	mov	si, DISK_ERR_MSG	;; Disk error message
	call	print

	xor	al, al
	mov	si, ax
	call	itos
	mov		si, di
	call	print			;; Error code

	mov	si, NEWLINE		;; Newline
	call	print
	
	hlt				;; Halt the bootloader

; SI - string to display.

print:
	lodsb				;; Load the next byte from SI into AL
	cmp	al, 0x0			;; Are we done?
	je	.done		
	mov	ah, 0xE 		;; BIOS Interrupt function code (0xE - display text(via VGA text mode))
	int	0x10			;; Interrupting BIOS
	jmp	print			;; loop
.done:
	ret

;; Wait for input(1 character), AL - CHARACTER
wfinput:
	mov	ah, 0x0			;; BIOS interrupt func code
	int	0x16			;; BIOS interrupt
	cmp	al, 0x0			;; If AL is 0x0, there is no input
	je	wfinput			;; Retry if no input
	ret

;; Integer to string (for debugging purposes)
;; SI - Integer, DI - String
itos:
	;; DI is going backwards btw

	mov	bx, 10			;; Decimal counting system
	mov	byte [di], 0		;; Adding null terminator
	dec	di
.loop:
	xor	dx, dx			;; Clearing the remainder of the last division
	mov	ax, si			;; setting quotient to SI
	div	bx			;; Dividing

	;;		DX:AX - dividend
	;;		AX - quotient
	;;		DX - remainder


	add	dl, '0'			;; Translating the remainder into an ASCII character
	mov	[di], dl		;; Adding the character
	dec	di			;; Moving to the next character

	mov	si, ax			;; Setting SI to the quotient of the division
	test	si, si			;; Is SI equal to 0?
	jnz	.loop			;; If it isn't, continue

	;; It is 0

	inc	di			;; Move the index up
	ret

;; Messages

INIT_MSG	db "HELLO", 0xD, 0xA, 0x0
DISK_ERR_MSG db "CAN'T READ DISK: ", 0x0
PROMPT_MSG	db "HDD (1-HDD1, 2-HDD2): ", 0xD, 0xA, 0x0
GRAPH_MSG   db  "COULD NOT ENABLE GRAPHICS MODE", 0xD, 0xA, 0x0

PROMPT2_MSG	db "GRAPHICS MODE? (1 - YES, OTHER - NO)", 0xD, 0xA ,0x0
NEWLINE		db 0xA, 0xD, 0x0

PROMPT_BUFF db 0x0, 0x0, 0x0, 0x0, 0x0

;; Data

BOOT_DRIVE	db 0x0

;; DAP
dap:
	db 0x10		;; DAP size
	db 0		;; Reserved
	db 0x7F		;; Sectors to read
	dd 0x1000   ;; Destination buffer
	dw 0x0000   ;; destination segment
	dq 0x2		;; Starting LBA

;; GDT

gdt_start:
gdt_null:
	dq	0x0
gdt_code:
	dw	0xFFFF
	dw	0x0
	db	0x0
	db	10011010b
	db	11001111b
	db	0x0
gdt_data:
	dw	0xFFFF
	dw	0x0
	db	0x0
	db	10010010b
	db	11001111b
	db	0x0
gdt_end:
gdt_descriptor:
	dw	gdt_end - gdt_start - 1
	dd	gdt_start

bits 32

;; For executing the kernel, that is 32 bit

protected_mode_exec:
	mov	ax, 0x10
	mov	ds, ax
	mov	es, ax
	mov	es, ax
	mov	ss, ax
	mov	fs, ax
	mov	gs, ax

	jmp	0x1000		;; Finally, jumping to the kernel

times 510 - ($ - $$) db 0

;; Boot signature

dw 0xAA55
