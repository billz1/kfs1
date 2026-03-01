; KFS1 multiboot entry + tiny boot stub (32-bit protected mode, entered by GRUB)
; NASM syntax (Intel)

BITS 32

SECTION .multiboot
    align 4
    dd 0x1BADB002              ; Multiboot magic
    dd 0x00000000              ; Flags (no special requests)
    dd -(0x1BADB002 + 0x00000000) ; Checksum

SECTION .text
global start
extern kmain

start:
    cli
    mov esp, stack_top
    call kmain
.hang:
    hlt
    jmp .hang

SECTION .bss
    align 16
stack_bottom:
    resb 16384                 ; 16 KiB stack
stack_top:

section .note.GNU-stack noalloc noexec nowrite progbits
