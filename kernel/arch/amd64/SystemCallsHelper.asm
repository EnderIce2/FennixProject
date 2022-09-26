; https://supercip971.github.io/02-wingos-syscalls.html

; +---+--------------------+
; | r |    Register(s)     |
; +---+--------------------+
; | a |   %eax, %ax, %al   |
; | b |   %ebx, %bx, %bl   |
; | c |   %ecx, %cx, %cl   |
; | d |   %edx, %dx, %dl   |
; | S |   %esi, %si        |
; | D |   %edi, %di        |
; +---+--------------------+

[BITS 64]
ALIGN	4096
extern syscall_handler
global syscall_handle
syscall_handle:
    swapgs
    mov [gs:0x8], rsp
    ; mov rsp, [gs:0x0]

    push qword 0x1b ; user data
    push qword [gs:0x8] ; saved stack
    push r11 ; saved rflags
    push qword 0x23 ; user code
    push rcx ; current IP

    cld
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp
    mov rbp, 0
    call syscall_handler

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx

    mov rsp, [gs:0x8]
    swapgs
    sti
    o64 sysret
