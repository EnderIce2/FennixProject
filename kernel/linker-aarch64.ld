ENTRY(_start)
 
SECTIONS
{
    . = 0x80000;

    _kernel_start = .;

    .text.boot :
    {
        *(.text.boot)
    }
    
    .text :
    {
        KEEP(*(.text.boot))
        *(.text)
    }
    . = ALIGN(4096);
    _kernel_text_end = .;

    .rodata :
    {
        *(.rodata)
    }
    . = ALIGN(4096);
    _kernel_rodata_end = .;

    .data :
    {
        *(.data)
    }
    _kernel_data_end = .;

    .bss :
    {
        bss = .;
        *(.bss)
    }
    . = ALIGN(4096);
    _kernel_bss_end = .;
    _kernel_bss_size = _kernel_bss_end - _kernel_data_end;
    _kernel_end = .;


    /DISCARD/ :
    {
        *(.eh_frame)
        *(.note .note.*)
    }
}
