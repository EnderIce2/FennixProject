global readTSC
readTSC:
    push rdx
    rdtsc
    shl rdx, 32
    or rax, rdx
    pop rdx
    ret
