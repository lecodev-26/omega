/*
 * OMEGA — Kernel
 * exceptions_asm.S — Vector table aarch64 (preemptiva)
 *
 * Guarda x0-x30 en el stack de la tarea antes de llamar a C.
 * Al retornar, si el scheduler cambió de tarea, hace context switch.
 */

.section .text.exceptions
.global exceptions_install
.global vector_table
.global exception_return_to_task

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
 * Stub común.
 *
 * Al entrar, sp apunta al stack de la tarea interrumpida.
 * Guardamos x0-x30 en ese stack.
 *
 * Después de llamar a C, si C decide cambiar de tarea, hace el
 * cambio de contexto directamente (modifica sp, ELR, SPSR).
 */
exc_stub_common:
    /* Reservar 256 bytes (32 * 8) para x0-x30 + padding */
    sub sp, sp, #256

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
     * x0 contenía el class_id. Lo guardamos en x19 para
     * pasarlo después de leer system registers.
     */
    mov x19, x0

    mrs x0, esr_el1
    mrs x1, elr_el1
    mrs x2, far_el1
    mrs x3, spsr_el1

    /* exception_handler_c(class, esr, elr, far, spsr) */
    /* x0=class, x1=esr, x2=elr, x3=far, x4=spsr */
    mov x4, x3     /* spsr */
    mov x3, x2     /* far */
    mov x2, x1     /* elr */
    mov x1, x0     /* esr */
    mov x0, x19    /* class */
    bl exception_handler_c

    /*
     * Después de C, sp puede haber cambiado si C hizo un context switch
     * (C puede haber modificado g_current y sp en el stack).
     *
     * Restauramos x0-x30 del stack actual.
     */
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

    add sp, sp, #256
    eret
