[bits 64]

[global _fxsave]
_fxsave:
	fxsave [rdi]
	ret


[global _fxrstor]
_fxrstor:
	fxrstor [rdi]
	ret
