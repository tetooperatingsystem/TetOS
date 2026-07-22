[BITS 16]
; SIIXX SEEEVEN
[ORG 0x7c00]

vars:		equ 0x7e00
line:		equ 0x7e80

_start:
	; syntax:
	;	first - title(number)
	;	like in basic:
	;	10   print "what if instead of wee it was wee wee"
	;	^^

	num	db "5", 0
	mov	si, num
	call	is_number

	mov	ah, 0x0E

	cmp	ax, 1
	je	f1

	mov	al, '0'
	int	0x10
	jmp	$
f1:
	mov	al, '1'
	int	0x10
	jmp	$

; To determine if a string holds a numerical value.
; Pass SI as the string and AX will be the output(0/1)
is_number:
	lodsb		; load the next byte from SI into AL
	
	; checking if we are done
	cmp	al, 0x0
	je	.hellyeah ; AMERICA! FUCK YEAH! AND THE STRING IS A NUMBER!!!

	cmp	al, '9' ; checking if its a digit
	jg	.nope
	cmp	al, '0'
	jl	.nope
.nope:
	mov	ax, 0
	ret

.hellyeah:
	mov	ax, 1
	ret

; Statements

run_statement:
	ret

print_statement:
	ret

statements:
	db 4, "new"
	dw _start
	db 4, "run"
	dw run_statement
	db 6, "print"
	dw print_statement

times	510 - ($ - $$) db 0
dw	0xAA55
