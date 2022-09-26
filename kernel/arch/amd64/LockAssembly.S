# https://en.wikipedia.org/wiki/Spinlock

.text
.global spinlock_lock
.global spinlock_unlock

# spinlock_lock(uint32_t *lock)
spinlock_lock: 
    xorq %rax,%rax
    lock btsl $0,(%rdi)
    jc spin
    ret
spin: 
    incq %rax
    cmpq $0x10000000,%rax
    je deadlock
    pause
    testl $1,(%rdi)
    jnz spin
    jmp spinlock_lock

# spinlock_unlock(uint32_t *lock)
spinlock_unlock: 
    lock btrl $0,(%rdi)
    ret

# deadlock_handler(LOCK *lock)
.extern deadlock_handler

deadlock: 
    pushq %rdi
    pushq %rdi
    xorq %rax,%rax
    call deadlock_handler
    popq %rdi
    popq %rdi
    jmp spin

.global spinlock_check_and_lock

# spinlock_check_and_lock(uint32_t *lock);
spinlock_check_and_lock: 
    xorl %eax,%eax
    lock btsl $0,(%rdi)
    setcb %al
    ret

.global spinlock_with_timeout

# spinlock_with_timeout(uint32_t *lock, uint64_t iterations);
spinlock_with_timeout: 
    xorq %rax,%rax
spin_timeout: 
    incq %rax
    lock btsl $0,(%rdi)
    setcb %bl
    cmpb $0,%bl
    je got_lock
    cmpq %rsi,%rax
    je timed_out
    pause
    jmp spin_timeout
got_lock: 
    movq $1,%rax
    ret
timed_out: 
    xorq %rax,%rax
    ret
