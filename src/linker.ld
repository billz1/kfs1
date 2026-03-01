/* Linker script for a Multiboot kernel loaded by GRUB */
ENTRY(start)

SECTIONS
{
    /* GRUB typically loads the kernel at 1 MiB */
    . = 1M;

    /* Multiboot header must be in the first 8 KiB of the kernel image */
    .multiboot ALIGN(4K) : {
        KEEP(*(.multiboot))
    }

    .text ALIGN(4K) : {
        *(.text*)
    }

    .rodata ALIGN(4K) : {
        *(.rodata*)
    }

    .data ALIGN(4K) : {
        *(.data*)
    }

    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss*)
    }
}
