Print:
	lodsb
	or	al, al
	jz	PrintDone
	mov	ah, 0eh
	int	10h
	jmp	Print
PrintDone:
	ret
