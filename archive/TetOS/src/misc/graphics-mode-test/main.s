[BITS 16]
[ORG 0x7c00]


_start:
	; entering graphics mode (mode 13h)

	mov	al, 0x12	; video mode
	mov	ah, 0x0		; function code

	int	0x10		; entering graphics mode

	mov	cx, 160
	mov	dx, 152
	mov	al, 'A'

	;call	draw_char

	mov	al, 0
	mov	bh, 0
	mov	dx, 0

	call	draw_lots_lines

	jmp	$


draw_lots_lines:
	mov	dx, 0
.loop:
	call	draw_vertical_line
	add	dx, 1
	cmp	dx, 480
	je	.done
	add	al, 1
	cmp	al, 16
	je	.loop_back_color
	jmp	.loop

.loop_back_color:
	mov	al, 0
	jmp	.loop
.done:
	ret

draw_vertical_line:
	mov	cx, 0
.loop:
	add	cx, 1
	cmp	cx, 640		; since we're in mode 13h, the width is 320 and height is 200
	je	.done
	call	draw_pixel
	jmp	.loop
.done:
	ret

; DX - Y, CX - X, AL = character
draw_char:
	; Character index

	mov	ax, dx
	mov	bx, 320
	
	mul	bx
	
	add	ax, cx
	mov	di, ax

	mov	bl, al
	sub	bl, 'A'
	mov	bh, 0
	shl	bx, 3	; *8
	
	mov	si, characters
	add	si, bx

	mov	bp, 8
.row:
	mov	al, [si]
	inc	si

	mov	cx, 8
.pixel:
	shl	al, 1
	jnc	.skip

	mov	byte [es:di], 15
.skip:
	inc	di
	loop	.pixel

	add	di, 320-8
	dec	bp
	jnz	.row

	ret

characters:
	font_H  db 00100001b ; 1
		db 00100001b ; 2
		db 00100001b ; 3
		db 00111111b ; 4
		db 00100001b ; 5
		db 00100001b ; 6
		db 00100001b ; 7
		db 00100001b ; 8

	font_E	db 00111111b ; 1
		db 00100000b ; 2
		db 00100000b ; 3
		db 00100000b ; 4
		db 00111111b ; 5
		db 00100000b ; 6
		db 00100000b ; 7
		db 00111111b ; 8

	font_L	db 00100000b ; 1
		db 00100000b ; 2
		db 00100000b ; 3
		db 00100000b ; 4
		db 00100000b ; 5
		db 00100000b ; 6
		db 00100000b ; 7
		db 00111111b ; 8

	font_O	db 01111110b ; 1
		db 10000001b ; 2
		db 10000001b ; 3
		db 10000001b ; 4
		db 10000001b ; 5
		db 10000001b ; 6
		db 10000001b ; 7
		db 01111110b ; 8

; CX - x, DX - y
; AL = color, BH = page number

draw_pixel:
	mov	ah, 0xC
	int	0x10
	ret

times 510 - ($ - $$) db 0
dw 0xAA55
