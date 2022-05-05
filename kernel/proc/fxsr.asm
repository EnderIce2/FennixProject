[bits 64]

[global fxsave]
fxsave:
	fxsave [rdi]
	ret


[global fxrstor]
fxrstor:
	fxrstor [rdi]
	ret
