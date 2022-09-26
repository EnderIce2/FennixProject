// https://wiki.osdev.org/Raspberry_Pi_Bare_Bones

.section ".text.boot"

.global _start
    .org 0x80000
_start:
    ldr     x5, =_start
    mov     sp, x5

    ldr     x5, =_kernel_data_end
    ldr     w6, =_kernel_bss_size
1:  cbz     w6, 2f
    str     xzr, [x5], #8
    sub     w6, w6, #1
    cbnz    w6, 1b
2:  bl      kernel_main_aarch64
halt:
    wfe
    b halt
