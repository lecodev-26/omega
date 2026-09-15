/*
 * OMEGA — Kernel
 * exceptions_asm.S — Vector table (preemptiva)
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
 */
exc_stub_common:
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

    /* Guardar class_id en x19 */
    mov x19, x0

    /* Leer system registers */
    mrs x0, esr_el1
    mrs x1, elr_el1
    mrs x2, far_el1
    mrs x3, spsr_el1

    /* exception_handler_c(class, esr, elr, far, spsr) */
    mov x4, x3
    mov x3, x2
    mov x2, x1
    mov x1, x0
    mov x0, x19
    bl exception_handler_c

    /*
     * Verificar si hay cambio de tarea pendiente.
     * g_preempt_next_idx es una variable global en C.
     */
    adrp x0, g_preempt_next_idx
    add  x0, x0, :lo12:g_preempt_next_idx
    ldr  w1, [x0]
    cmp  w1, #0
    b.lt 1f              /* si < 0, retorno normal */

    /* Hay cambio. Llamar a context_switch_from_irq(from, to). */

    /* from = task_context_of(g_preempt_from_idx) */
    adrp x0, g_preempt_from_idx
    add  x0, x0, :lo12:g_preempt_from_idx
    ldr  w0, [x0]
    bl   task_context_of
    mov  x19, x0         /* guardar from */

    /* to = task_context_of(g_preempt_next_idx) */
    adrp x0, g_preempt_next_idx
    add  x0, x0, :lo12:g_preempt_next_idx
    ldr  w0, [x0]
    bl   task_context_of
    mov  x1, x0          /* x1 = to */

    mov  x0, x19         /* x0 = from */
    bl   context_switch_from_irq
    /* Nunca retorna: hace eret */

1:
    /* Retorno normal: restaurar x0-x30 y eret */
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
