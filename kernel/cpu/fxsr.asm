[bits 64]

[global _amd64_fxsave]
_amd64_fxsave:
	fxsave [rdi]
	ret

[global _amd64_fxrstor]
_amd64_fxrstor:
	fxrstor [rdi]
	ret

[bits 32]

[global _i686_fxsave]
_i686_fxsave:
	fxsave [edi]
	ret

[global _i686_fxrstor]
_i686_fxrstor:
	fxrstor [edi]
	ret
