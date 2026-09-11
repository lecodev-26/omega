/*
 * OMEGA — Kernel
 * task.c — Tareas y scheduler (preparado para preemption)
 */

#include <stddef.h>
#include "omega/task.h"
#include "omega/ipc.h"
#include "omega/cap.h"
#include "omega/uart.h"

extern void context_switch(task_context_t *from, task_context_t *to);
extern void task_start_first(task_context_t *to);
extern void task_trampoline(void);
extern void task_trampoline_irq(void);

static task_t g_tasks[MAX_TASKS] __attribute__((aligned(16)));
static int    g_num_tasks = 0;
static int    g_current = -1;

void task_init(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        g_tasks[i].idx = (uint64_t)i;
        g_tasks[i].state = TASK_STATE_UNUSED;
        g_tasks[i].yields = 0;
        g_tasks[i].irqs = 0;
        g_tasks[i].name[0] = '\0';
    }
    g_num_tasks = 0;
    g_current = -1;
}

int task_create(const char *name, void (*entry)(void)) {
    if (g_num_tasks >= MAX_TASKS) return -1;
    if (entry == NULL) return -1;

    int idx = g_num_tasks++;
    task_t *t = &g_tasks[idx];
    t->idx = (uint64_t)idx;

    int i = 0;
    while (name[i] != '\0' && i < TASK_NAME_MAX - 1) {
        t->name[i] = name[i];
        i++;
    }
    t->name[i] = '\0';

    t->state = TASK_STATE_READY;
    t->entry = entry;
    t->yields = 0;
    t->irqs = 0;

    /* Limpiar todos los registros */
    for (int j = 0; j < 31; j++) {
        t->context.x[j] = 0;
    }

    /* SP al tope del stack, alineado a 16 */
    uintptr_t sp_top = (uintptr_t)(t->stack + TASK_STACK_SIZE);
    sp_top &= ~((uintptr_t)0xF);
    t->context.sp = sp_top;

    /* PC: por ahora usamos el trampoline cooperativo */
    t->context.pc = (uint64_t)task_trampoline;

    /* SPSR: EL1h, DAIF enmascarado (interrupciones deshabilitadas) */
    t->context.spsr = 0x5 | (0x3 << 6);

    /* x19 lleva el puntero a la task (usado por el trampoline) */
    t->context.x[19] = (uint64_t)t;

    ipc_register_endpoint((uint32_t)idx);
    cap_table_init((uint32_t)idx);

    return idx;
}

void task_entry_point(task_t *t) {
    if (t && t->entry) {
        t->entry();
    }
}

void task_finished(void) {
    if (g_current >= 0) {
        g_tasks[g_current].state = TASK_STATE_FINISHED;
    }
    for (;;) {
        task_yield();
    }
}

int task_schedule_next(void) {
    if (g_num_tasks == 0) return -1;

    int start = (g_current + 1) % g_num_tasks;
    int idx = start;
    do {
        if (g_tasks[idx].state == TASK_STATE_READY ||
            g_tasks[idx].state == TASK_STATE_RUNNING) {
            g_current = idx;
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

    if (next < 0) return;

    if (prev == next) {
        g_tasks[prev].yields++;
        return;
    }

    if (prev < 0) {
        /* Primera llamada: arrancar directamente la siguiente tarea */
        g_tasks[next].state = TASK_STATE_RUNNING;
        g_tasks[next].yields++;
        task_start_first(&g_tasks[next].context);
        return;
    }

    g_tasks[prev].state = TASK_STATE_READY;
    g_tasks[prev].yields++;
    g_tasks[next].state = TASK_STATE_RUNNING;
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

/* ============================================================
 * Preemption
 * ============================================================ */

int g_preempt_next_idx = -1;

/*
 * Llamado desde el handler de IRQ.
 * Decide si cambiar de tarea. NO hace el cambio directamente.
 * Actualiza g_preempt_next_idx con el índice de la nueva tarea.
 */
void task_tick_from_irq(void) {
    if (g_num_tasks == 0) {
        g_preempt_next_idx = -1;
        return;
    }

    /* Incrementar el contador de IRQs de la tarea actual */
    if (g_current >= 0) {
        g_tasks[g_current].irqs++;
    }

    /* Round-robin: elegir la siguiente tarea */
    int prev = g_current;
    int start = (g_current + 1) % g_num_tasks;
    int idx = start;
    int next = -1;

    do {
        if (g_tasks[idx].state == TASK_STATE_READY ||
            g_tasks[idx].state == TASK_STATE_RUNNING) {
            next = idx;
            break;
        }
        idx = (idx + 1) % g_num_tasks;
    } while (idx != start);

    if (next < 0 || next == prev) {
        g_preempt_next_idx = -1;
        return;
    }

    /* Actualizar estados */
    if (prev >= 0) {
        g_tasks[prev].state = TASK_STATE_READY;
    }
    g_tasks[next].state = TASK_STATE_RUNNING;

    /* Actualizar g_current ANTES de retornar al assembly */
    g_current = next;

    /* Indicar al assembly que hay que cambiar de tarea */
    g_preempt_next_idx = next;
}

task_context_t *task_context_of(int idx) {
    if (idx < 0 || idx >= g_num_tasks) return NULL;
    return &g_tasks[idx].context;
}

void task_set_current(int idx) {
    g_current = idx;
}
