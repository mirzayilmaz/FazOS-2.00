/* FazOS Kernel - Multiboot 1 giris noktasi (x86, 32 bit) */
.set MAGIC, 0x1BADB002
.set FLAGS, 3
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long -(MAGIC+FLAGS)
.section .bss
.align 16
stack_bottom: .skip 16384
stack_top:
.section .text
.global _start
_start:
    mov $stack_top, %esp
    push %ebx
    push %eax
    call kmain
1:  cli
    hlt
    jmp 1b
