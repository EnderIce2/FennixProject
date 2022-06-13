global ReadPITCounter
ReadPITCounter:
    mov al, 0x0
    out 0x43, al
    in al, 0x40
    mov ah, al
    in al, 0x40
    rol ax, 8
    ret
