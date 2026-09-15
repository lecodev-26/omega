/*
 * OMEGA — Kernel
 * switch.S — Cambio de contexto
 *
 * Layout del task_context_t:
 *   offset 0     : x[0]
 *   offset 8     : x[1]
 *   ...
 *   offset 152   : x[19]
 *   ...
 *   offset 240   : x[30]
 *   offset 248   : sp
 *   offset 256   : pc
 *   offset 264   : spsr
 */

.section .text

/* ============================================================
 * void context_switch(task_context_t *from, task_context_t *to)
 *
 * Cambio de contexto cooperativo: guarda/restaura solo callee-saved
 * (x19-x30), SP y PC.
 * ============================================================ */
.global context_switch
context_switch:
    /* Guardar x19-x30 en `from` */
    stp x19, x20, [x0, #152]
    stp x21, x22, [x0, #168]
    stp x23, x24, [x0, #184]
    stp x25, x26, [x0, #200]
    stp x27, x28, [x0, #216]
    stp x29, x30, [x0, #232]

    /* Guardar SP actual */
    mov x2, sp
    str x2, [x0, #248]

    /* Guardar PC */
    str x30, [x0, #256]

    /* Cargar x19-x30 de `to` */
    ldp x19, x20, [x1, #152]
    ldp x21, x22, [x1, #168]
    ldp x23, x24, [x1, #184]
    ldp x25, x26, [x1, #200]
    ldp x27, x28, [x1, #216]
    ldp x29, x30, [x1, #232]

    /* Cargar SP */
    ldr x2, [x1, #248]
    mov sp, x2

    /* Saltar al PC */
    ldr x2, [x1, #256]
    br x2

/* ============================================================
 * void task_start_first(task_context_t *to)
 *
 * Arranca una tarea desde cero.
 * ============================================================ */
.global task_start_first
task_start_first:
    ldp x19, x20, [x0, #152]
    ldp x21, x22, [x0, #168]
    ldp x23, x24, [x0, #184]
    ldp x25, x26, [x0, #200]
    ldp x27, x28, [x0, #216]
    ldp x29, x30, [x0, #232]

    ldr x2, [x0, #248]
    mov sp, x2

    ldr x2, [x0, #256]
    br x2

/* ============================================================
 * task_trampoline: entrada de tareas nuevas (cooperativo)
 * ============================================================ */
.global task_trampoline
task_trampoline:
    mov x0, x19
    bl task_entry_point
    bl task_finished
1:
    wfe
    b 1b

/* ============================================================
 * void context_switch_from_irq(task_context_t *from, task_context_t *to)
 *
 * Cambio de contexto desde el stub de IRQ.
 *
 * Se asume que el SP actual apunta a un marco de 256 bytes que
 * contiene x0-x30 de la tarea interrumpida (guardados por el stub).
 *
 * Flujo:
 *   1. Copiar x0-x30 del stack a `from->context.x[0..30]`.
 *   2. Guardar SP (sp + 256), PC (ELR_EL1), SPSR en `from`.
 *   3. Cambiar SP al marco de `to`.
 *   4. Cargar PC y SPSR de `to`.
 *   5. Cargar x0-x30 desde el marco al CPU.
 *   6. Ajustar SP al SP original de la tarea.
 *   7. eret.
 * ============================================================ */
.global context_switch_from_irq
context_switch_from_irq:
    /* --- Guardar x0-x30 del stack actual a `from` --- */
    ldp x2, x3, [sp, #0]
    stp x2, x3, [x0, #0]
    ldp x2, x3, [sp, #16]
    stp x2, x3, [x0, #16]
    ldp x2, x3, [sp, #32]
    stp x2, x3, [x0, #32]
    ldp x2, x3, [sp, #48]
    stp x2, x3, [x0, #48]
    ldp x2, x3, [sp, #64]
    stp x2, x3, [x0, #64]
    ldp x2, x3, [sp, #80]
    stp x2, x3, [x0, #80]
    ldp x2, x3, [sp, #96]
    stp x2, x3, [x0, #96]
    ldp x2, x3, [sp, #112]
    stp x2, x3, [x0, #112]
    ldp x2, x3, [sp, #128]
    stp x2, x3, [x0, #128]
    ldp x2, x3, [sp, #144]
    stp x2, x3, [x0, #144]
    ldp x2, x3, [sp, #160]
    stp x2, x3, [x0, #160]
    ldp x2, x3, [sp, #176]
    stp x2, x3, [x0, #176]
    ldp x2, x3, [sp, #192]
    stp x2, x3, [x0, #192]
    ldp x2, x3, [sp, #208]
    stp x2, x3, [x0, #208]
    ldp x2, x3, [sp, #224]
    stp x2, x3, [x0, #224]
    ldr x2, [sp, #240]
    str x2, [x0, #240]

    /* Guardar SP de la tarea (sp + 256) */
    add x2, sp, #256
    str x2, [x0, #248]

    /* Guardar PC (ELR_EL1) y SPSR */
    mrs x2, elr_el1
    str x2, [x0, #256]
    mrs x2, spsr_el1
    str x2, [x0, #264]

    /* --- Cambiar SP al marco de `to` --- */
    ldr x2, [x1, #248]        // x2 = to->sp
    sub x2, x2, #256          // x2 = to->sp - 256
    mov sp, x2

    /* --- Cargar PC y SPSR de `to` --- */
    ldr x2, [x1, #256]
    msr elr_el1, x2
    ldr x2, [x1, #264]
    msr spsr_el1, x2
    isb

    /* --- Cargar x0-x30 de `to` en el stack (marco actual) --- */
    ldp x2, x3, [x1, #0]
    stp x2, x3, [sp, #0]
    ldp x2, x3, [x1, #16]
    stp x2, x3, [sp, #16]
    ldp x2, x3, [x1, #32]
    stp x2, x3, [sp, #32]
    ldp x2, x3, [x1, #48]
    stp x2, x3, [sp, #48]
    ldp x2, x3, [x1, #64]
    stp x2, x3, [sp, #64]
    ldp x2, x3, [x1, #80]
    stp x2, x3, [sp, #80]
    ldp x2, x3, [x1, #96]
    stp x2, x3, [sp, #96]
    ldp x2, x3, [x1, #112]
    stp x2, x3, [sp, #112]
    ldp x2, x3, [x1, #128]
    stp x2, x3, [sp, #128]
    ldp x2, x3, [x1, #144]
    stp x2, x3, [sp, #144]
    ldp x2, x3, [x1, #160]
    stp x2, x3, [sp, #160]
    ldp x2, x3, [x1, #176]
    stp x2, x3, [sp, #176]
    ldp x2, x3, [x1, #192]
    stp x2, x3, [sp, #192]
    ldp x2, x3, [x1, #208]
    stp x2, x3, [sp, #208]
    ldp x2, x3, [x1, #224]
    stp x2, x3, [sp, #224]
    ldr x2, [x1, #240]
    str x2, [sp, #240]

    /* --- Cargar x0-x30 del stack a los registros --- */
    ldp x0, x1, [sp, #0]
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
    ldp x2, x3, [sp, #16]

    /* Ajustar SP al SP original de la tarea */
    add sp, sp, #256

    /* eret */
    eret
