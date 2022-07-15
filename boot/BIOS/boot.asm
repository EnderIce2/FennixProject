[ORG 0x7C00]
[BITS 16]

start:
    jmp	0x0000:boot
    nop

times 8-($-$$) db 0
    ; Boot Information Table
    bi_PrimaryVolumeDescriptor      dd 0 ; LBA of the Primary Volume Descriptor
    bi_BootFileLocation             dd 0 ; LBA of the Boot File
    bi_BootFileLength               dd 0 ; Length of the boot file in bytes
    bi_Checksum                     dd 0 ; 32 bit checksum
    bi_Reserved            times 40 db 0 ; Reserved 'for future standardization'
times 90-($-$$) db 0

boot:
	cli
	xor ax, ax
    mov ds, ax
    mov si, UnsupportedString
    call Print
    jmp $

Print:
NextChar:
    mov al, [si]
    inc si
    mov ah, 0x0e
    mov bh, 0x00
    mov bl, 0x07
    int 0x10
    or al, al
    jz Exit
    jmp NextChar
Exit:
    ret

UnsupportedString db "BIOS boot is not supported.", 0 
