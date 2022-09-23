[ORG 0x7C00]
[BITS 16]

start:
    jmp	0x0000:Boot
    nop

times 8-($-$$) db 0
PrimaryVolumeDescriptor dd 0
BootFileLocation dd 0
BootFileLength dd 0
Checksum dd 0
Reserved times 40 db 0
times 90-($-$$) db 0

%include "print.inc"

Boot:
	cli
    mov [BOOT_DISK], dl
	xor ax, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x9C00
    mov si, BootloaderText
    call Print
    call ReadDisk
    jmp EX_ADDRESS
    jmp $

ReadDisk:
    sti
    mov ah, 0x02
    mov bx, EX_ADDRESS
    mov al, 20 ; max 65
    mov dl, [BOOT_DISK]
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02
    int 0x13
    jc DiskError
    cli
    ret

DiskError:
    cli
    mov si, DiskReadingErrorMessage
    call Print
    jmp $

BootloaderText db 'Lynx Bootloader', 0
DiskReadingErrorMessage: db ' Disk Error', 0
EX_ADDRESS equ 0x8000
BOOT_DISK: db 0

times 510-($-$$) db 0
db 0x55
db 0xAA
