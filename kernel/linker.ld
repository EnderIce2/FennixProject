OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

ENTRY(kernel_entry)

SECTIONS
{
    . = 0xffffffff80000000;

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
}
