; This should be at 0x0, 0xdeadbeef, 0xdeadcode, etc... Or there is a better way to handle invalid addresses?
; Not tested!
global _deadaddress_start
_deadaddress_start:
    mov rax, qword infinite_loop_deadaddress
    call rax

global _deadaddress_end
_deadaddress_end:


section .text
infinite_loop_deadaddress:
    call infinite_loop_deadaddress
