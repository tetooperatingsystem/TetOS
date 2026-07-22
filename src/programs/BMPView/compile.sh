
# ASM

nasm -f elf32 bmp.s -o build/bmp.o

# C

clang --target=i386-elf -ffreestanding -c main.c -o build/main.o -nostdlib
clang --target=i386-elf -ffreestanding -c include/fat16.c -o build/fat16.o -nostdlib

ld -m elf_i386 -T linker.ld build/bmp.o build/main.o build/fat16.o -o build/bmpview.elf
objcopy -O binary build/bmpview.elf build/BMPView.BIN
