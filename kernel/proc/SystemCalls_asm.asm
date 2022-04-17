extern syscalls_handler
global syscall_handler_helper
syscall_handler_helper:
    mov qword [gs:16], rsp ; Store User Stack
    mov rsp, qword [gs:8] ; Restore Kernel Stack
    pushaq ; Push All Registers
    cld ; Disable Interrupts
    mov rdi, rsp ; ? Copy Kernel Stack ?
    call syscalls_handler ; Call The Handler
    popaq ; Pop All Registers
    ; Returning from the syscall
    push 0x23 ; Push SS Segment
    push qword [gs:16] ; Restore User Stack
    push r11 ; RFLAGS
    push 0x1B ; CS
    push rcx ; RIP
    iretq ; Return and enable interrupts
