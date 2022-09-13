OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)

ENTRY(kernel_entry)

PHDRS
{
    null    PT_NULL    FLAGS(0) ;                   /* Null segment */
    text    PT_LOAD    FLAGS((1 << 0) | (1 << 2)) ; /* Execute + Read */
    rodata  PT_LOAD    FLAGS((1 << 2)) ;            /* Read only */
    data    PT_LOAD    FLAGS((1 << 1) | (1 << 2)) ; /* Write + Read */
}

SECTIONS
{
    . = 0x100000;

    _kernel_start = .;
    .text :
    {
        *(.text .text.*)
    } :text

    _kernel_text_end = ALIGN(CONSTANT(MAXPAGESIZE));
    . += CONSTANT(MAXPAGESIZE);

    .rodata :
    {
        *(.rodata .rodata.*)
    } :rodata

    _kernel_rodata_end = ALIGN(CONSTANT(MAXPAGESIZE));
    . += CONSTANT(MAXPAGESIZE);

    .data :
    {
        *(.data .data.*)
    } :data

    .bss :
    {
        *(COMMON)
        *(.bss .bss.*)
    } :data

    . += CONSTANT(MAXPAGESIZE);
    _kernel_end = ALIGN(CONSTANT(MAXPAGESIZE));

    /DISCARD/ :
    {
        *(.eh_frame)
        *(.note .note.*)
    }
}
