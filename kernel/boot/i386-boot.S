/* ---------------------------------------------------------------------------------------------------------------------- */
.set MAGIC,     0xe85250d6                  // magic number
.set ARCH,      0                           // architecture
.set LENGTH,    (HdrEnd - HdrStart)         // header length
.set CHECKSUM,  -(MAGIC + ARCH + LENGTH)    // checksum

.section .multiboot.data, "aw"
.align 0x1000
HdrStart:
.long MAGIC
.long ARCH
.long LENGTH
.long CHECKSUM

/*
.align 8
TagFramebufferStart:
.short 5
.short 1
.short TagFramebufferEnd - TagFramebufferStart
.short 1920
.short 1080
.short 32
TagFramebufferEnd:
*/

.align 8
TagsStart:
.short 0
.short 0
.short TagsEnd - TagsStart
TagsEnd:
HdrEnd:

.section .bootstrap_stack, "aw", @nobits
BootstrapStackBottom:
.skip 16384
BootstrapStackTop:

.section .multiboot.text, "a"

.intel_syntax noprefix
.global CheckCPUID
CheckCPUID:
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    xor eax, ecx
    jz .NoCPUID
    mov eax, 1
    ret
.NoCPUID:
    xor eax, eax
    ret

.global CheckLongMode
CheckLongMode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .NoLongMode
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .NoLongMode
    mov eax, 1
    ret
.NoLongMode:
    xor eax, eax
    ret
.att_syntax prefix

.global CallHigherHalf
CallHigherHalf:
    lea 4f, %ecx
    jmp *%ecx
.section .text
4:
    movl %cr3, %ecx
    movl %ecx, %cr3
    mov $BootstrapStackTop, %esp
    call kernel_entry
    cli
1:
    hlt
    jmp 1b
