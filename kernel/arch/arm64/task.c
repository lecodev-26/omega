/*
 * OMEGA — Kernel
 * task.c — Tareas y scheduler (preemptivo)
 */

#include <stddef.h>
#include <stdint.h>
#include "omega/task.h"
#include "omega/uart.h"
#include "omega/ipc.h"
#include "omega/cap.h"

extern void context_switch(task_context_t *from, task_context_t *to);
extern void task_start_first(task_context_t *to);
extern void task_trampoline(void);

static task_t g_tasks[MAX_TASKS];
static int    g_num_tasks = 0;
static int    g_current   = -1;

int g_preempt_from_idx = -1;
int g_preempt_next_idx = -1;

void task_init(void) {
    g_num_tasks = 0;
    g_current = -1;
    g_preempt_from_idx = -1;
    g_preempt_next_idx = -1;

    for (int i = 0; i < MAX_TASKS; i++) {
        g_tasks[i].state = TASK_STATE_UNUSED;
        g_tasks[i].idx = (uint64_t)i;
        g_tasks[i].yields = 0;
        g_tasks[i].irqs = 0;
    }
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

    for (int j = 0; j < 31; j++) {
        t->context.x[j] = 0;
    }

    uintptr_t sp_top = (uintptr_t)(t->stack + TASK_STACK_SIZE);
    sp_top &= ~((uintptr_t)0xF);
    t->context.sp = sp_top;

    t->context.pc = (uint64_t)task_trampoline;

    /*
     * SPSR: EL1h (0x5) + DAIF con I=0 (IRQs habilitadas).
     *
     *   0x305 = 0b11_0000_0101:
     *     bits [3:0] = 0101 → EL1h (SP_EL1)
     *     bit  [4]   = 0    → AArch64
     *     bits [9:6] = 1100 → D=1, A=1, I=0, F=0
     */
    t->context.spsr = 0x305;

    t->context.x[19] = (uint64_t)t;

    /*
     * Preparar el stack de la tarea como si hubiera sido interrumpida:
     * escribir x0-x30 en el marco de 256 bytes.
     */
    uintptr_t frame_sp = sp_top - 256;
    uint64_t *frame = (uint64_t *)frame_sp;
    for (int j = 0; j < 31; j++) {
        frame[j] = 0;
    }
    frame[19] = (uint64_t)t;

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
    task_yield();
    for (;;) {
        __asm__ volatile("wfe");
    }
}

int task_schedule_next(void) {
    if (g_num_tasks == 0) return -1;
    if (g_num_tasks == 1) return 0;

    int start = (g_current + 1) % g_num_tasks;
    int idx = start;

    do {
        if (g_tasks[idx].state == TASK_STATE_READY ||
            g_tasks[idx].state == TASK_STATE_RUNNING) {
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
    if (g_current < 0 || g_current >= g_num_tasks) return NULL;
    return &g_tasks[g_current];
}

task_t *task_get(int idx) {
    if (idx < 0 || idx >= g_num_tasks) return NULL;
    return &g_tasks[idx];
}

task_context_t *task_context_of(int idx) {
    if (idx < 0 || idx >= g_num_tasks) return NULL;
    return &g_tasks[idx].context;
}

void task_set_current(int idx) {
    g_current = idx;
}

void task_tick_from_irq(void) {
    if (g_num_tasks == 0) {
        g_preempt_from_idx = -1;
        g_preempt_next_idx = -1;
        return;
    }

    g_preempt_from_idx = g_current;

    if (g_current >= 0) {
        g_tasks[g_current].irqs++;
    }

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

    if (prev >= 0) {
        g_tasks[prev].state = TASK_STATE_READY;
    }
    g_tasks[next].state = TASK_STATE_RUNNING;
    g_current = next;
    g_preempt_next_idx = next;
}
