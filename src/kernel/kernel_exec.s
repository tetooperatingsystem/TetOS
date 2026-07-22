;
;   This file is just for executing the kernel
;   So... yeah.
;

[BITS 32]

extern kernel_main

_start:
    ; I would like to add a debug message, but we're in protected mode

    call    kernel_main