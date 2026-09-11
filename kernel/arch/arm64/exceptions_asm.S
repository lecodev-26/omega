/*
 * OMEGA — Boot minimal
 * exceptions_asm.S — Vector table aarch64 (full context save)
 */

.section .text.exceptions
.global exceptions_install
.global vector_table

.align 11

vector_table:
    .align 7
    b exc_stub_sync
    .align 7
    b exc_stub_irq
    .align 7
    b exc_stub_fiq
    .align 7
    b exc_stub_serror

    .align 7
    b exc_stub_sync
    .align 7
    b exc_stub_irq
    .align 7
    b exc_stub_fiq
    .align 7
    b exc_stub_serror

    .align 7
    b exc_stub_sync
    .align 7
    b exc_stub_irq
    .align 7
    b exc_stub_fiq
    .align 7
    b exc_stub_serror

    .align 7
    b exc_stub_sync
    .align 7
    b exc_stub_irq
    .align 7
    b exc_stub_fiq
    .align 7
    b exc_stub_serror

exceptions_install:
    ldr x0, =vector_table
    msr vbar_el1, x0
    isb
    ret

exc_stub_sync:
    mov x0, #1
    b exc_stub_common

exc_stub_irq:
    mov x0, #2
    b exc_stub_common

exc_stub_fiq:
    mov x0, #3
    b exc_stub_common

exc_stub_serror:
    mov x0, #4
    b exc_stub_common

/*
 * Stub común: guarda TODOS los registros x0-x30 antes de llamar a C.
 * Reservamos 256 bytes en el stack para x0-x30 + LR + padding.
 */
exc_stub_common:
    /* Reservar 272 bytes (17 * 16 = 272, alineado a 16) */
    sub sp, sp, #272

    /* Guardar x0-x30 */
    stp x0, x1, [sp, #0]
    stp x2, x3, [sp, #16]
    stp x4, x5, [sp, #32]
    stp x6, x7, [sp, #48]
    stp x8, x9, [sp, #64]
    stp x10, x11, [sp, #80]
    stp x12, x13, [sp, #96]
    stp x14, x15, [sp, #112]
    stp x16, x17, [sp, #128]
    stp x18, x19, [sp, #144]
    stp x20, x21, [sp, #160]
    stp x22, x23, [sp, #176]
    stp x24, x25, [sp, #192]
    stp x26, x27, [sp, #208]
    stp x28, x29, [sp, #224]
    str x30, [sp, #240]

    /*
     * Ahora x0 sigue conteniendo el class_id original (no se ha modificado
     * porque lo guardamos en el stack y solo guardamos, no sobrescribimos).
     *
     * Leer system registers a x1-x4.
     */
    mrs x1, esr_el1
    mrs x2, elr_el1
    mrs x3, far_el1
    mrs x4, spsr_el1

    /* Llamar a exception_handler_c(x0, x1, x2, x3, x4) */
    bl exception_handler_c

    /* Restaurar x0-x30 */
    ldp x0, x1, [sp, #0]
    ldp x2, x3, [sp, #16]
    ldp x4, x5, [sp, #32]
    ldp x6, x7, [sp, #48]
    ldp x8, x9, [sp, #64]
    ldp x10, x11, [sp, #80]
    ldp x12, x13, [sp, #96]
    ldp x14, x15, [sp, #112]
    ldp x16, x17, [sp, #128]
    ldp x18, x19, [sp, #144]
    ldp x20, x21, [sp, #160]
    ldp x22, x23, [sp, #176]
    ldp x24, x25, [sp, #192]
    ldp x26, x27, [sp, #208]
    ldp x28, x29, [sp, #224]
    ldr x30, [sp, #240]

    add sp, sp, #272

    /* Return from exception: eret vuelve a ELR_EL1 */
    eret
