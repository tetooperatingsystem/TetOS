clang -ffreestanding --target=i386-elf -nostdlib -c main.c -o main.o

ld -m elf_i386 -T linker.ld main.o -o main.elf
objcopy -O binary main.elf main.bin
