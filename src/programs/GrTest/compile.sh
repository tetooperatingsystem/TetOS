clang --target=i386-elf -ffreestanding -c main.c -o build/main.o -nostdlib

ld -m elf_i386 -T linker.ld build/main.o -o build/grtest.elf

objcopy -O binary build/grtest.elf build/GrTest.BIN
