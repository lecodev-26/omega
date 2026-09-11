/*
 * OMEGA — Boot minimal
 * linker.ld
 *
 * Linker script para binario bare-metal aarch64.
 * El binario se carga en 0x40000000 (donde QEMU virt carga los kernels).
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

    /* Vector table alineada explícitamente a 0x800 (2048 bytes) */
    . = ALIGN(2048);
    .text.exceptions : {
        *(.text.exceptions)
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
