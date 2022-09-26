[bits 32]

[global _i686_fxsave]
_i686_fxsave:
	fxsave [edi]
	ret

[global _i686_fxrstor]
_i686_fxrstor:
	fxrstor [edi]
	ret
