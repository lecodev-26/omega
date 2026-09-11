/*
 * OMEGA — Boot minimal
 * exceptions_asm.S — Vector table aarch64
 *
 * 16 slots, uno por cada (source EL × tipo de excepción):
 *   - Current EL with SP0
 *   - Current EL with SPx
 *   - Lower EL AArch64
 *   - Lower EL AArch32
 * Cada grupo: SYNC, IRQ, FIQ, SError.
 */

.section .text.exceptions
.global exceptions_install
.global vector_table

.align 11

vector_table:
    /* Current EL with SP0 */
    .align 7
    b exc_stub_sync
    .align 7
    b exc_stub_irq
    .align 7
    b exc_stub_fiq
    .align 7
    b exc_stub_serror

    /* Current EL with SPx */
    .align 7
    b exc_stub_sync
    .align 7
    b exc_stub_irq
    .align 7
    b exc_stub_fiq
    .align 7
    b exc_stub_serror

    /* Lower EL using AArch64 */
    .align 7
    b exc_stub_sync
    .align 7
    b exc_stub_irq
    .align 7
    b exc_stub_fiq
    .align 7
    b exc_stub_serror

    /* Lower EL using AArch32 */
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
    mov x0, #1      /* OMEGA_EXC_SYNC */
    b exc_stub_common

exc_stub_irq:
    mov x0, #2      /* OMEGA_EXC_IRQ */
    b exc_stub_common

exc_stub_fiq:
    mov x0, #3      /* OMEGA_EXC_FIQ */
    b exc_stub_common

exc_stub_serror:
    mov x0, #4      /* OMEGA_EXC_SERROR */
    b exc_stub_common

exc_stub_common:
    /* Reservar 48 bytes y guardar x1..x4 + x30 */
    sub sp, sp, #48
    stp x1, x2, [sp, #0]
    stp x3, x4, [sp, #16]
    str x30, [sp, #32]

    /* Leer system registers */
    mrs x1, esr_el1
    mrs x2, elr_el1
    mrs x3, far_el1
    mrs x4, spsr_el1

    /* Llamar a exception_handler_c(x0, x1, x2, x3, x4) */
    bl exception_handler_c

    /* Restaurar */
    ldr x30, [sp, #32]
    ldp x3, x4, [sp, #16]
    ldp x1, x2, [sp, #0]
    add sp, sp, #48

    /* Bucle infinito: no recuperamos */
1:
    wfe
    b 1b
