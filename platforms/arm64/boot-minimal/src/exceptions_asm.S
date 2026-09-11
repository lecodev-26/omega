/*
 * OMEGA — Boot minimal
 * exceptions_asm.S — Vector table aarch64
 */

.section .text.exceptions
.global exceptions_install
.global vector_table

.align 11

vector_table:
    .rept 16
    .align 7
    b exc_default_handler
    .endr

exceptions_install:
    ldr x0, =vector_table
    msr vbar_el1, x0
    isb
    ret

exc_default_handler:
    /* Marca de vida: '!' */
    mov w0, #0x21
    ldr x1, =0x09000000
1:
    ldr w2, [x1, #0x18]
    tbnz w2, #5, 1b
    str w0, [x1]
2:
    wfe
    b 2b
