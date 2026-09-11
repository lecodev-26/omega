/*
 * OMEGA — Boot minimal
 * linker.ld
 *
 * Linker script para binario bare-metal aarch64.
 *
 * Este binario se carga en la dirección física 0x40000000,
 * que es donde QEMU virt carga los kernels.
 */

ENTRY(_start)

SECTIONS
{
    . = 0x40000000;

    .text : {
        *(.text.boot)
        *(.text)
        *(.text.*)
    }

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    .data : {
        *(.data)
        *(.data.*)
    }

    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }

    . = ALIGN(4096);
    __stack_top = .;
}
