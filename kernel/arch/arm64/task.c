/*
 * OMEGA — Kernel
 * task.c — Tareas y scheduler cooperativo (13a)
 */

#include <stddef.h>
#include "omega/task.h"
#include "omega/uart.h"

/* Importados de switch.S */
extern void context_switch(task_context_t *from, task_context_t *to);
extern void task_trampoline(void);
extern void task_finished(void);

static task_t g_tasks[MAX_TASKS];
static int    g_num_tasks = 0;
static int    g_current = -1;

/* Usado por el trampoline para saber qué task ejecutar. */

void task_init(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        g_tasks[i].state = TASK_STATE_UNUSED;
        g_tasks[i].name[0] = '\0';
        g_tasks[i].yields = 0;
    }
    g_num_tasks = 0;
    g_current = -1;
}

int task_create(const char *name, void (*entry)(void)) {
    if (g_num_tasks >= MAX_TASKS) return -1;
    if (entry == NULL) return -1;

    int idx = g_num_tasks++;
    task_t *t = &g_tasks[idx];

    int i = 0;
    while (name[i] != '\0' && i < TASK_NAME_MAX - 1) {
        t->name[i] = name[i];
        i++;
    }
    t->name[i] = '\0';

    t->state = TASK_STATE_READY;
    t->entry = entry;
    t->yields = 0;

    /* Limpiar contexto */
    t->context.x19 = 0;
    t->context.x20 = 0;
    t->context.x21 = 0;
    t->context.x22 = 0;
    t->context.x23 = 0;
    t->context.x24 = 0;
    t->context.x25 = 0;
    t->context.x26 = 0;
    t->context.x27 = 0;
    t->context.x28 = 0;
    t->context.x29 = 0;
    t->context.x30 = 0;

    /*
     * Inicializar SP: apunta al tope del stack de la tarea.
     * El stack crece hacia abajo. Reservamos 16 bytes para
     * alinear a 16 bytes (requisito AAPCS64).
     */
    uintptr_t sp_top = (uintptr_t)(t->stack + TASK_STACK_SIZE);
    sp_top &= ~((uintptr_t)0xF);   /* alinear a 16 */
    t->context.sp = sp_top;

    /*
     * Inicializar PC: apunta al trampoline.
     * Cuando esta tarea sea programada por primera vez, saltará
     * al trampoline, que llamará a entry().
     */
    t->context.pc = (uint64_t)task_trampoline;

    /*
     * x19: convención para pasar el puntero a la task al trampoline.
     * El trampoline espera x19 = puntero a task.
     */
    t->context.x19 = (uint64_t)t;

    return idx;
}

/*
 * task_entry_point(task) — llamada desde el trampoline.
 * Extrae entry() del struct task y la ejecuta.
 */
void task_entry_point(task_t *t) {
    if (t && t->entry) {
        t->entry();
    }
}

/*
 * task_finished() — llamada desde el trampoline si entry() retorna.
 */
void task_finished(void) {
    if (g_current >= 0) {
        g_tasks[g_current].state = TASK_STATE_FINISHED;
    }
    /* Ceder el control para siempre (la siguiente tarea arrancará) */
    for (;;) {
        task_yield();
    }
}

int task_schedule_next(void) {
    if (g_num_tasks == 0) return -1;

    /* Buscar la siguiente tarea en estado READY */
    int start = (g_current + 1) % g_num_tasks;
    int idx = start;
    do {
        if (g_tasks[idx].state == TASK_STATE_READY ||
            g_tasks[idx].state == TASK_STATE_RUNNING) {
            g_current = idx;
            g_tasks[idx].state = TASK_STATE_RUNNING;
            return idx;
        }
        idx = (idx + 1) % g_num_tasks;
    } while (idx != start);

    return -1;
}

void task_yield(void) {
    if (g_num_tasks == 0) return;

    int prev = g_current;
    int next = task_schedule_next();

    if (next < 0) {
        /* No hay siguiente tarea; seguir con la actual */
        return;
    }

    if (prev == next) {
        /* Solo hay una tarea; no hace falta cambiar contexto */
        g_tasks[prev].yields++;
        return;
    }

    if (prev >= 0) {
        g_tasks[prev].state = TASK_STATE_READY;
    }

    g_tasks[next].yields++;

    context_switch(&g_tasks[prev].context, &g_tasks[next].context);
}

int task_count(void) {
    return g_num_tasks;
}

task_t *task_current(void) {
    if (g_current < 0) return NULL;
    return &g_tasks[g_current];
}

task_t *task_get(int idx) {
    if (idx < 0 || idx >= g_num_tasks) return NULL;
    return &g_tasks[idx];
}
