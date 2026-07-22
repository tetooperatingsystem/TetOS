ENTRY(_start)

SECTIONS
{
	. = 0x0;
	.start : { *(.start) }
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	.bss : { *(.bss) }
}
