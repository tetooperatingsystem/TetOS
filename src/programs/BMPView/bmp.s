
bits 32

extern read_

;; Definitions.


%define BI_RLE8          1      ;;  BITMAPINFOHEADER from Windows identifier for 8 bits per pixel
%define BMP_H_SIZE       14     ;;  BMP Header Size

;; Labels.

parsed_ptr     dd  0           ;; Pointer to parsed
bmp_ptr        dd  0x90000     ;; Pointer to BMP

term_row       dw  0           ;; Terminal Row
term_col       dw  0           ;; Terminal Column

framebuffer_ptr dd  0           ;; Pointer to framebuffer
data_ptr       dd  0           ;; Pointer to bitmap

img_width      dw  0           ;; Image Width
img_height     dw  0           ;; Image Height

;; Labels for drawing

dr_x           dd  0           ;; X
dr_y           dd  0           ;; Y

;; Structures.

struc bmp_header
    .identifier resb 2          ;;  'BM' in ASCII to identify BMP files.
    .size      resd 1          ;;  Size of the BMP file in bytes
    .reserved  resw 2          ;;  Reserved; depends on application.
    .address   resd 1          ;;  Address of the byte where the bitmap starts.
endstruc

struc old_dib_header
    .h_size    resd 1          ;;  Header size (12 bytes)
    .width     resw 1          ;;  Bitmap Width  (Unsigned)
    .height    resw 1          ;;  Bitmap Height (Unsigned)
    .color_pl  resw 1          ;;  Number of Color Planes
    .b_per_p   resw 1          ;;  Bits per pixel (we only support 8)
endstruc

struc new_dib_header
    .h_size    resd 1          ;;  Size of this header (40 bytes)
    .width     resd 1          ;;  Bitmap Width  (Signed)
    .height    resd 1          ;;  Bitmap Height (Signed)
    .color_pl  resw 1          ;;  Number of color planes (Should be 1)
    .b_per_p   resw 1          ;;  Bits Per Pixel
    .comp_meth resd 1          ;;  Compression Method. we only support BI_RLE8 that is 0x1
    .img_size  resd 1          ;;  Size of the raw bitmap data; could be 0 for BI_RGB
    .hor_res   resd 1          ;;  Horizontal resolution of the image (pixel per metre, signed)
    .ver_res   resd 1          ;;  Vertical resolution of the image (pixel per metre, signed)
    .col_count resd 1          ;;  Number of colors in the color palette
    .imp_color resd 1          ;;  Number of important colors used, 0 when all colors are important; usually ignored.
endstruc

struc vesa_header
    ;; Most of this doesn't matter
    .doesntm   resb 16
    .width     resw 1
    .height    resw 1
    .doesntm2  resb 18
    .framebuff  resd 1
    .doesntm3  resb 212
endstruc

;; SI - x, DI - y, CL - color
setpixel:
    ;; Convert X and Y to 32-bit for further math.
    movzx   esi, si
    movzx   edi, di

    mov     eax, edi            ;; i=y
    push    ecx                 ;; Preserve color
    mov     ecx, 640
    mul     ecx                 ;; i=y*w
    add     eax, esi            ;; i=y*w+x
    add     eax, [framebuffer_ptr];; i=framebuffer+y*w+x
    pop     ecx
    mov     [eax], cl           ;; finally draw

    ret

draw:

;; dr_y - Line
.draw_line:
    ;; dr_x - X

    mov     eax, dr_x           ;; Move X and Y to 32-bit registers for Math.
    mov     esi, dr_x

    ;; Multiply Y by Image Width.
    ;; Index formula: i=(y*w)+x

    mov     ecx, [img_width]
    mul     ecx                 ;; eax=y*w
    add     eax, esi            ;; eax=(y*w)+x
    add     eax, [data_ptr]       

    mov     cl, [eax]           ;; move color into CL
    mov     si, [esi]           ;; X
    mov     ebx, dr_y
    mov     di, [ebx]           ;; Y

    call    setpixel            ;; Draw Pixel.

    mov     esi, dr_x           ;; Add 1 to X
    
    inc     dword [dr_x]

    mov     eax, dr_x
    cmp     eax, [img_width]
    
    jne     .draw_line

    mov     dword dr_x, 0
    inc     dword [dr_y]    

    mov     eax, dr_y
    cmp     eax, [img_height]
    jne     .draw_line

.done:
    ret

_start:
    mov     [parsed_ptr], esi   ;; Parsed[32][32] is in parsed_ptr

    call    read_               ;; Call the C part. Now the BMP file is at 0x90000

    ;; mov     [term_row], [0x1002]
    ;; mov     [term_col], [0x1000]

    mov     esi, 0x80000
    ;; EDI - Pointer to framebuffer
    mov     edi, [esi + vesa_header.framebuff]
    mov     [framebuffer_ptr], edi

    ;; Get bmp data start address
    mov     esi, 0x90000
    mov     edi, [esi + bmp_header.address]
    add     edi, esi
    mov     [data_ptr], edi

    ;; Get width and height
    
    ;; First, let's find out what DIB header we are using.
    mov     esi, 0x90000
    mov     edi, [esi + BMP_H_SIZE + old_dib_header.h_size]
    cmp     edi, 12
    je      .old_dib
    cmp     edi, 40
    je      .new_dib

    ret

;; Although I hate this,
;; New DIB header from Microsoft uses Signed 32-bit, (even though width and height cannot be negative)
;; Whilst old DIB header from OS/2 uses Unsigned 16-bit

.new_dib:
    ;; Width
    mov     esi, 0x90000
    mov     edi, [esi + BMP_H_SIZE + new_dib_header.width]
    mov     [img_width], edi
    ;; Height
    mov     esi, 0x90000
    mov     edi, [esi + BMP_H_SIZE + new_dib_header.height]
    mov     [img_height], edi

    jmp     .continue

.old_dib:
    ;; Width
    mov     esi, 0x90000
    mov     di, [esi + BMP_H_SIZE + old_dib_header.width]
    mov     [img_width], di
    ;; Height
    mov     esi, 0x90000
    mov     di, [esi + BMP_H_SIZE + old_dib_header.height]
    mov     [img_height], di

    jmp     .continue
.continue:
    call    draw
end_program:
    ret