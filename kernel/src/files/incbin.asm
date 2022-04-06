%macro inc_bin 2
	SECTION .rodata
	GLOBAL %1
	GLOBAL %1_size
%1:
	incbin %2
	db 0
	%1_size: dq %1_size - %1
%endmacro

inc_bin zap_ext_light24, "./files/zap_ext_light24.psf"
inc_bin zap_ext_vga16, "./files/zap_ext_vga16.psf"
