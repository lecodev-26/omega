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
 *   x0 = from
 *   x1 = to
 *
 * Cambio de contexto cooperativo: guarda/restaura solo callee-saved
 * (x19-x30), SP y PC.
 * ============================================================ */
.global context_switch
context_switch:
    /* Guardar x19-x30 en `from` (offsets 152..240) */
    stp x19, x20, [x0, #152]
    stp x21, x22, [x0, #168]
    stp x23, x24, [x0, #184]
    stp x25, x26, [x0, #200]
    stp x27, x28, [x0, #216]
    stp x29, x30, [x0, #232]

    /* Guardar SP actual */
    mov x2, sp
    str x2, [x0, #248]

    /* Guardar PC (dirección de retorno = x30) */
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
 *   x0 = to
 *
 * Arranca una tarea desde cero. Carga su contexto y salta al PC.
 * No retorna.
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
 *
 * Al arrancar una tarea nueva, x19 contiene el puntero a la task.
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
 * task_trampoline_irq: entrada de tareas nuevas (preemptivo)
 * Reservado para 18b/18c. Por ahora no se usa.
 * ============================================================ */
.global task_trampoline_irq
task_trampoline_irq:
    /* Reservado */
1:
    wfe
    b 1b
