.set MAGIC, 0x1badb002   #magic numbers that show to bootloader that it is a kernel
.set FLAGS, (1<<0 | 1<<1)
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM


.section .text
.extern kernelMain #function kernel main in kernel.cpp
.extern callConstructors
.global loader


loader:
    mov $kernel_stack, %esp
    call callConstructors
    push %eax
    push %ebx
    call kernelMain


_stop:   #infinite loop for our kernel
    cli
    hlt
    jmp _stop


.section .bss
.space 2*1024*1024; #size of stack
kernel_stack:   #stack pointer of os
