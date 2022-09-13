; From: https://github.com/Supercip971/WingOS/blob/master/kernel/arch/x86_64/smp_start.asm

%ifdef AMD64

[bits 16]
TRAMPOLINE_BASE equ 0x2000

; --- 16 BIT ---
global nstack
extern StartCPU
global _trampoline_start
_trampoline_start:
    cli
mov al, 'a'
mov dx, 0x3F8   ; these are used for debugging
out dx, al
    mov ax, 0x0
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
mov al, 'b'
mov dx, 0x3F8   ; these are used for debugging
out dx, al
    o32 lgdt [pm_gdtr - _trampoline_start + TRAMPOLINE_BASE]
mov al, 'c'
mov dx, 0x3F8
out dx, al
    mov eax, cr0
    or al, 0x1
    mov cr0, eax
mov al, 'd'
mov dx, 0x3F8
out dx, al
    jmp 0x8:(trampoline32 - _trampoline_start + TRAMPOLINE_BASE)

[bits 32]
section .text
; ---- 32 BITS ----
trampoline32:
    mov bx, 0x10
    mov ds, bx
    mov es, bx
    mov ss, bx
    mov eax, dword [0x500]
    mov cr3, eax
mov al, 'e'
mov dx, 0x3F8
out dx, al
    mov eax, cr4
    or eax, 1 << 5 ; Set the PAE-bit, which is the 6th bit (bit 5).
    or eax, 1 << 7
    mov cr4, eax
    mov ecx, 0xc0000080
    rdmsr
    or eax,1 << 8  ; LME
    wrmsr
mov al, 'f'
mov dx, 0x3F8
out dx, al
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
mov al, 'g'
mov dx, 0x3F8
out dx, al
    lgdt [lm_gdtr - _trampoline_start + TRAMPOLINE_BASE]
mov al, 'h'
mov dx, 0x3F8
out dx, al
    jmp 0x8:(trampoline64 - _trampoline_start + TRAMPOLINE_BASE)

[bits 64]
; ---- 64 BITS ----
trampoline64:
mov al, 'i'
mov dx, 0x3F8
out dx, al
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov ax, 0x0
    mov fs, ax
    mov gs, ax
mov al, 'j'
mov dx, 0x3F8
out dx, al
    lgdt [0x580]
    lidt [0x590]
    mov rsp, [0x570]
mov al, 'k'
mov dx, 0x3F8
out dx, al
    mov rbp, 0x0 ; terminate stack traces here
    ; reset RFLAGS
    push 0x0
    popf
mov al, 'l'
mov dx, 0x3F8
out dx, al
    mov rax, qword vcode64
    call vcode64

vcode64:
mov al, 'm'
mov dx, 0x3F8
out dx, al
    push rbp
    ; set up sse as higher half use it
    mov rax, cr0
mov al, 'n'
mov dx, 0x3F8
out dx, al
    ; btr eax, 2
    ; bts eax, 1
    ; mov cr0, rax
mov al, 'o'
mov dx, 0x3F8
out dx, al
    mov rax, cr4
    bts eax, 9
    bts eax, 10
    mov cr4, rax
mov al, 'p'
mov dx, 0x3F8
out dx, al
    mov rax, qword trampoline_ext
    call rax

; ---- LONG MODE ----
align 16
  lm_gdtr:
    dw lm_gdt_end - lm_gdt_start - 1
    dq lm_gdt_start - _trampoline_start + TRAMPOLINE_BASE

align 16
  lm_gdt_start:
    ; null selector
    dq 0
    ; 64bit cs selector
    dq 0x00AF98000000FFFF
    ; 64bit ds selector
    dq 0x00CF92000000FFFF
  lm_gdt_end:

; ---- Protected MODE ----
align 16
  pm_gdtr:
    dw pm_gdt_end - pm_gdt_start - 1
    dd pm_gdt_start - _trampoline_start + TRAMPOLINE_BASE

align 16
  pm_gdt_start:
    ; null selector
    dq 0
    ; cs selector
    dq 0x00CF9A000000FFFF
    ; ds selector
    dq 0x00CF92000000FFFF
  pm_gdt_end:
; IDT
align 16
  pm_idtr:
    dw 0
    dd 0
    dd 0
    align 16

global _trampoline_end
_trampoline_end:

trampoline_ext:
mov al, 'q'
mov dx, 0x3F8
out dx, al
mov al, ' '
mov dx, 0x3F8
out dx, al
mov al, 'S'
mov dx, 0x3F8
out dx, al
mov al, 'U'
mov dx, 0x3F8
out dx, al
mov al, 'C'
mov dx, 0x3F8
out dx, al
mov al, 'C'
mov dx, 0x3F8
out dx, al
mov al, 'E'
mov dx, 0x3F8
out dx, al
mov al, 'S'
mov dx, 0x3F8
out dx, al
mov al, 'S'
mov dx, 0x3F8
out dx, al
mov al, 0x0A
mov dx, 0x3F8
out dx, al
    call StartCPU

%endif

times 512 - ($-$$) db 0
