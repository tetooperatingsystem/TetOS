[BITS 16]
[ORG 0x1000]

;	   Fat16 Data structure
;
;	__________________________
;	| 	Boot Sector	 |
;	|________________________|
;	|	BPB & EBPB	 |
;	|________________________|
;	|	 FAT #0		 |
;	|________________________|
;	|	 FAT #1		 |
;	|________________________|
;	|     Root Directory	 |
;	|________________________|
;	|        Data Area	 |
;	|________________________|
;
; Note : FAT #0 is usually the original table,
; whilst FAT #1 is a backup
;
; A FAT is a Table that contains a value for each cluster to see if its allocated or not.
;	Values:
;		0x0000	-	Not Allocated
;		0xFFF7	-	Bad Cluster
;	0xFFF8 and greater -	End of FAT
;		Rest	-	Allocated sector
;
;
;BPB:
;	Offset	Size	Meaning
;	0x00	3	JMP SHORT 3C (needed for Windows and OSX)
;	0x03	8	OEM identifier
;	0x0B	2	the number of bytes per sector (little-endian)
;	0x0D	1	sectors per cluster
;	0x0E	2	number of reserved sectors (boot sectors included)
;	0x10	1	number of file allocation tables (oftenly, 2)	
;	0x11	2	number of root directory entries (must be set so that the root directory occupies entire sectors)
;	0x13	2	the total sectors in the logical volume (if the value is 0, it means there are more than 2^16 sectors in the volume, and the actual count is in the LSC entry at 0x20)
;	0x15	1	media descriptor type
;	0x16	2	number of sectors per FAT
;	0x18	2	number of sectors per track
;	0x1A	2	number of heads or sides on the storage media
;	0x1C	4	number of hidden sectors
;	0x20	4	large sector count.
;EBPB:
;	0x24	1	drive number
;	0x25	1	flags in windows NT. reserved otherwise
;	0x26	1	signature (0x28 OR 0x29)
;	0x27	4	volumeid 'serial' number. (non necessary)
;	0x2B	11	volume label string. padded with spaces.
;	0x36	8	system identifier string. string representation of the FAT file system type. it is padded with spaces. the spec says to never trust teh contents of this string for any use
;	0x3E	448	boot code.
;	0x1FE	2	bootable partition signature  0xAA55
;Directory:
;	0x00	11	Short File Name
;	0x0B	1	File attributes (See table below)
;	0x0C	1	Reserved by Windows NT
;	0x0D	1	Creation time(hundreds of a second)
;	0x0E	2	Creation time(hour, minute, second/2)
;	0x10	2	Creation date(year, month, date)
;	0x12	2	Last accessdate(year, month, day)
;	0x14	2	High word of the first  cluster index (on FAT12 and FAT16 - 0)
;	0x16	2	Modification time(hour, minute, second)
;	0x18	2	Modification date(year, month, day)
;	0x1A	2	First cluster number for this entry
;	0x16	4	Size in bytes
;Filename Entry:
;	0x0	1	Entry type = 0xC1
;	0x1	1	Flags
;	0x2	30	File name characters (15 UTF16 code units).
;
;
; A sector is usually 512 bytes, whilst a directory entry is 32 bytes.
; So unless the sector is 4096 bytes, a sector should contain 16 directories.
;
;


_start:
	mov	si, Init_MSG
	call	printf

	; Reading BPB

	mov	al, 0
	mov	ah, 0
	mov	es, ax

	mov	dl, 0x81	; drive
	mov	bx, 0x1200	; destination
	mov	al, 0x1		; number of sectors
	mov	ch, 0x0		; cylinders
	mov	cl, 0x1		; sector (starts with 1!)
	mov	dh, 0x0		; head

	mov	ah, 0x02	; read sectors
	int	0x13
	jc	disk_error


	;	Reading BPS

	call	read_bpb

	;	DEBUG

	call	debug

	jmp	$

read_bpb:
	mov	ax, [0x120B]
	mov	[BPS], ax

	;	SPC
	xor	ah, ah
	mov	al, [0x120D]
	mov	[SPC], ax

	;	Amount of Root Directory Entries
	mov	ax, [0x1211]
	mov	[ROOT_DIRS_AMT], ax

	;	Amount of Sectors

	mov	ax, [0x1213]
	mov	[TOTAL_SECTORS], ax

	;	Reserved Sectors
	mov	ax, [0x120E]
	mov	[RESERV_SECTORS], ax

	;	Sectors per FAT
	mov	ax, [0x1216]
	mov	[SPF], ax
	
	;	Amount of FATs
	mov	al, [0x1210]
	mov	[N_FATS], al

	ret

get_offset_root_dir:
	;	Root_Dir_Sectors = (ROOT_DIR_AMT * 32) / BPS
	mov	ax, [ROOT_DIRS_AMT]
	mov	cx, 32
	mul	cx
	xor	dx, dx
	div	word [BPS]
	mov	[Root_Dir_Sectors], ax
	
	;	First_Root_Sector = RESERV_SECTORS + (N_FATS * SPF)
	mov	al, [N_FATS]
	cbw
	mul	word [SPF]
	add	ax, [RESERV_SECTORS]
	mov	[First_Root_Sector], ax

	ret

debug:
	; BPS

	mov	si, [BPS]
	mov	di, NUM_BUFF + 5
	call	itos
	mov	si, BPS_MSG
	call	dbg_print

	;  Sector count
	mov	si, [TOTAL_SECTORS]
	mov	di, NUM_BUFF + 5
	call	itos
	mov	si, SECTORS_MSG
	call	dbg_print

	;  SPC
	mov	si, [SPC]
	mov	di, NUM_BUFF + 5
	call	itos
	mov	si, SPC_MSG
	call	dbg_print

	; Reserves
	mov	si, [RESERV_SECTORS]
	mov	di, NUM_BUFF + 5
	call	itos
	mov	si, RESERV_MSG
	call	dbg_print
	; SPF
	mov	si, [SPF]
	mov	di, NUM_BUFF + 5
	call	itos
	mov	si, SPF_MSG
	call	dbg_print

	; N_FATS
	mov	si, [N_FATS]
	mov	di, NUM_BUFF + 3
	call	itos
	mov	si, NFATS_MSG
	call	dbg_print

	ret


;	SI - string 1, DI - string 2
dbg_print:
	call	printf
	mov	si, di
	call	printf
	mov	si, NEWLINE_MSG
	call	printf
	ret

; SI - string
printf:
	lodsb
	or	al, al
	jz	.done
	mov	ah, 0x0E
	int	0x10
	jmp	printf
.done:
	ret


; si - integer, di - string

itos:
	mov	bx, 10
	mov	byte [di], 0
	dec	di
.loop:
	xor	dx, dx
	mov	ax, si
	div	bx

	;	DX:AX - Dividend, AX - quotient, DX - remainder

	add	dl, '0'
	mov	[di], dl
	dec	di

	mov	si, ax
	test	si, si
	jnz	.loop

	inc	di
	ret

disk_error:
	mov	si, Err_MSG
	call	printf
	jmp	$

; misc

; MSGs

SPC_MSG		db	"SPC ", 0x0
BPS_MSG		db	"BPS ", 0x0
Err_MSG		db	"FAT16: Disk error.", 0xD, 0xA, 0x0
NEWLINE_MSG	db	0xA, 0xD, 0x0
Init_MSG	db	"FAT16: Init!", 0x0D, 0xA, 0x0
RESERV_MSG	db	"RESERVS ", 0x0
SPF_MSG		db	"SPF ", 0x0
NFATS_MSG	db	"FATS ", 0x0
SECTORS_MSG	db	"SECTORS ", 0x0
; BPB

BPS		dw	0x0000		; Bytes Per Second (0x0B-0x0C)
SPC		dw	0x0000		; Sectors Per Cluster (0x0D) !!! This is supposed to be a byte, but it gets wrong values if its a byte, and it doesnt when its a word, so....
RESERV_SECTORS	dw	0x0000		; Reserved Sectors (0x0E-0x0F)
SPF		dw	0x0000		; Sectors per FAT (0x16-0x17)
N_FATS		db	0x0		; Amount of FATs (0x10)
NUM_BUFF	times 6 db 0
TOTAL_SECTORS	dw	0x0000		; Amount of Sectors
ROOT_DIRS_AMT	dw	0x0000		; Amount of root directory entries

Root_Dir_Sectors dw	0x0000		; Sector of the root dir		
First_Root_Sector dw	0x0000		; First sector of ^^^^^^
Data_Offset_Sector dw	0x0000		; Offset of data
times	512 - ($ - $$) db 0

