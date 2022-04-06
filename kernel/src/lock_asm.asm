[bits 64]
; https://en.wikipedia.org/wiki/Spinlock
section .text
global spinlock_lock
global spinlock_unlock

; spinlock_lock(uint32_t *lock)
spinlock_lock:
    xor rax, rax
    lock bts dword [rdi], 0
    jc spin
    ret
spin:
    inc rax
    cmp rax, 0x10000000
    je deadlock
    pause
    test dword [rdi], 1
    jnz spin
    jmp spinlock_lock

; spinlock_unlock(uint32_t *lock)
spinlock_unlock:
    lock btr dword [rdi], 0
    ret

; deadlock_handler(LOCK *lock)
extern deadlock_handler

deadlock:
    push rdi
    push rdi
    xor rax, rax
    call deadlock_handler
    pop rdi
    pop rdi
    jmp spin

global spinlock_check_and_lock

; spinlock_check_and_lock(uint32_t *lock);
spinlock_check_and_lock:
    xor eax, eax
    lock bts dword [rdi], 0
    setc al
    ret

global spinlock_with_timeout

; spinlock_with_timeout(uint32_t *lock, uint64_t iterations);
spinlock_with_timeout:
    xor rax, rax
spin_timeout:
    inc rax
    lock bts dword [rdi], 0
    setc bl
    cmp bl, 0
    je got_lock
    cmp rax, rsi
    je timed_out
    pause
    jmp spin_timeout
got_lock:
    mov rax, 1
    ret
timed_out:
    xor rax, rax
    ret