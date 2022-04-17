global asmRand16
asmRand16:
    db 0xf, 0xc7, 0xf0
    jc r16ok
    xor rax, rax
    ret
r16ok:
    mov [rcx], ax
    mov rax,  1
    ret

global asmRand32
asmRand32:
    db 0xf, 0xc7, 0xf0
    jc r32ok
    xor rax, rax
    ret
r32ok:
    mov [rcx], eax
    mov rax,  1
    ret

global asmRand64
asmRand64:
    db 0x48, 0xf, 0xc7, 0xf0
    jc r64ok
    xor rax, rax
    ret
r64ok:
    mov [rcx], rax
    mov rax, 1
    ret

