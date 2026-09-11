/*
 * OMEGA — Kernel
 * switch.S — Cambio de contexto cooperativo
 *
 * void context_switch(task_context_t *from, task_context_t *to);
 *   x0 = from
 *   x1 = to
 *
 * Guarda x19-x30, SP y PC de `from`, carga los de `to`.
 *
 * Convención AAPCS64: x19-x28, x29 (FP), x30 (LR) son callee-saved.
 * SP es también responsabilidad del callee.
 */

.section .text
.global context_switch

context_switch:
    /* Guardar x19-x30 en `from` */
    stp x19, x20, [x0, #0]
    stp x21, x22, [x0, #16]
    stp x23, x24, [x0, #32]
    stp x25, x26, [x0, #48]
    stp x27, x28, [x0, #64]
    stp x29, x30, [x0, #80]

    /* Guardar SP actual */
    mov x2, sp
    str x2, [x0, #96]

    /* Guardar PC: es la dirección de retorno, es decir, LR original.
     * Como `context_switch` fue llamada con `bl`, x30 contiene la
     * dirección de retorno en el caller. Ya lo guardamos arriba.
     * Copiamos x30 a pc. */
    str x30, [x0, #104]

    /* Cargar x19-x30 de `to` */
    ldp x19, x20, [x1, #0]
    ldp x21, x22, [x1, #16]
    ldp x23, x24, [x1, #32]
    ldp x25, x26, [x1, #48]
    ldp x27, x28, [x1, #64]
    ldp x29, x30, [x1, #80]

    /* Cargar SP de `to` */
    ldr x2, [x1, #96]
    mov sp, x2

    /* Cargar PC de `to` y saltar */
    ldr x2, [x1, #104]

    /* Saltar a la dirección del PC guardado.
     * No usamos `ret` porque ret usa x30, y ya cargamos x30 arriba. */
    br x2

/*
 * task_trampoline() — punto de entrada de tareas nuevas.
 *
 * El scheduler guarda la dirección de esta función como PC inicial
 * de cada tarea nueva. Cuando la tarea arranca por primera vez,
 * saltamos aquí. La tarea real corre como una sub-llamada.
 *
 * x19 contiene el puntero al struct task (lo configuramos al crear).
 */
.global task_trampoline
.extern task_entry_point

task_trampoline:
    /* x19 ya contiene el puntero a la task */
    /* Llamar a task_entry_point(task) */
    mov x0, x19
    bl task_entry_point

    /* Si retorna, la tarea terminó. */
    bl task_finished

    /* Nunca debería llegar aquí, pero por si acaso */
1:
    wfe
    b 1b

/*
 * task_start_first(task_context_t *to) — arranca la primera tarea.
 * No hay tarea previa. Solo cargamos el contexto de `to` y saltamos.
 */
.global task_start_first
task_start_first:
    /* x0 = puntero a task_context_t de la tarea a arrancar */

    /* Cargar x19-x30 */
    ldp x19, x20, [x0, #0]
    ldp x21, x22, [x0, #16]
    ldp x23, x24, [x0, #32]
    ldp x25, x26, [x0, #48]
    ldp x27, x28, [x0, #64]
    ldp x29, x30, [x0, #80]

    /* Cargar SP */
    ldr x2, [x0, #96]
    mov sp, x2

    /* Cargar PC y saltar */
    ldr x2, [x0, #104]
    br x2
