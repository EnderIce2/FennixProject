; TODO
init:
    mov si, LoadingText
    call Print
    jmp $

%include "print.inc"

LoadingText db ' Loading...', 0
