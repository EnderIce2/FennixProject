[bits 64]

[global _amd64_fxsave]
_amd64_fxsave:
	fxsave [rdi]
	ret

[global _amd64_fxrstor]
_amd64_fxrstor:
	fxrstor [rdi]
	ret
