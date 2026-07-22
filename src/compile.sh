# kernel
clang --target=i386-elf -ffreestanding -c kernel/kernel.c -o ../build/kernel.o -nostdlib

# drivers and other

clang --target=i386-elf -ffreestanding -c drivers/vga/vga.c -o ../build/vga.o -nostdlib # VGA
clang --target=i386-elf -ffreestanding -c drivers/vesa/vesa.c -o ../build/vesa.o -nostdlib # VESA
clang --target=i386-elf -ffreestanding -c terminal/terminal.c -o ../build/terminal.o -nostdlib # TERMINAL
clang --target=i386-elf -ffreestanding -c kernel/ports.c -o ../build/ports.o -nostdlib # PORT MANAGEMENT
clang --target=i386-elf -ffreestanding -c drivers/ps2/keyboard/kb_ps2.c -o ../build/kb_ps2.o -nostdlib # PS2 KEYBOARD
clang --target=i386-elf -ffreestanding -c kernel/drive_io.c -o ../build/drive_io.o -nostdlib # DRIVE I/O
clang --target=i386-elf -ffreestanding -c drivers/fat16/fat16.c -o ../build/fat16.o -nostdlib # FAT16
clang --target=i386-elf -ffreestanding -c kernel/idt/idt.c -o ../build/idt.o -nostdlib # IDT
clang --target=i386-elf -ffreestanding -c kernel/idt/interrupts.c -o ../build/interrupts.o -nostdlib # IDT Interrupts
clang --target=i386-elf -ffreestanding -c drivers/cmos/cmos.c -o ../build/cmos.o -nostdlib # CMOS
clang --target=i386-elf -ffreestanding -c rand.c -o ../build/rand.o -nostdlib # RANDOM
clang --target=i386-elf -ffreestanding -c drivers/fonts/psf.c -o ../build/psf.o -nostdlib # PC System FONT
clang --target=i386-elf -ffreestanding -c drivers/sound/sound.c -o ../build/sound.o -nostdlib # Sound(PCSPKR)
clang --target=i386-elf -ffreestanding -c drivers/wait.c -o ../build/wait.o -nostdlib 
nasm -f elf32 kernel/idt/interrupts.s -o ../build/interrupts_s.o
# shell

clang --target=i386-elf -ffreestanding -c shell/shell.c -o ../build/shell.o -nostdlib # SHELL
clang --target=i386-elf -ffreestanding -c shell/shell_func.c -o ../build/shell_func.o -nostdlib # SHELL_FUNC
# memory shit

clang --target=i386-elf -ffreestanding -c mem.c -o ../build/mem.o -nostdlib		# memory management and sheet

# bootloader
nasm -f bin bootloader/boot.s -o ../build/boot.bin
nasm -f elf32 kernel/kernel_exec.s -o ../build/kernel_exec.o

ld -m elf_i386 -T kernel/linker.ld ../build/kernel_exec.o ../build/kernel.o ../build/rand.o ../build/interrupts_s.o ../build/sound.o ../build/psf.o ../build/vesa.o ../build/vga.o ../build/cmos.o ../build/terminal.o ../build/ports.o ../build/kb_ps2.o ../build/shell.o ../build/shell_func.o ../build/mem.o ../build/drive_io.o ../build/fat16.o ../build/wait.o  ../build/idt.o ../build/interrupts.o -o ../build/kernel.elf 
objcopy -O binary ../build/kernel.elf ../build/kernel.bin

truncate -s 65024 ../build/kernel.bin
