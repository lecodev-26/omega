/*
 * OMEGA — Kernel
 * task.h — Tareas y scheduler (preparado para preemption)
 */

#ifndef OMEGA_TASK_H
#define OMEGA_TASK_H

#include <stdint.h>

#define TASK_NAME_MAX   32
#define TASK_STACK_SIZE 4096
#define MAX_TASKS       8

typedef enum {
    TASK_STATE_UNUSED = 0,
    TASK_STATE_READY,
    TASK_STATE_RUNNING,
    TASK_STATE_FINISHED
} task_state_t;

/*
 * Contexto de una tarea.
 *
 * Guarda x0-x30 + SP + PC + SPSR. Este es el contexto necesario
 * para reanudar una tarea interrumpida por una IRQ.
 *
 * Alineado a 16 bytes para evitar Alignment faults.
 */
typedef struct {
    uint64_t x[31];      /* x0 - x30 */
    uint64_t sp;         /* SP_EL1 */
    uint64_t pc;         /* ELR_EL1 */
    uint64_t spsr;       /* SPSR_EL1 */
} __attribute__((aligned(16))) task_context_t;

typedef struct {
    uint64_t       idx;
    uint64_t       yields;
    uint64_t       irqs;
    task_context_t context;
    void         (*entry)(void);
    task_state_t   state;
    char           name[TASK_NAME_MAX];
    uint8_t        stack[TASK_STACK_SIZE];
} __attribute__((aligned(16))) task_t;

void task_init(void);
int  task_create(const char *name, void (*entry)(void));
void task_yield(void);
int  task_count(void);
task_t *task_current(void);
task_t *task_get(int idx);

/*
 * Punto de entrada de tareas nuevas (llamado por el trampoline).
 */
void task_entry_point(task_t *t);

/*
 * Llamada cuando una tarea termina (su entry retorna).
 */
void task_finished(void);

#endif /* OMEGA_TASK_H */

/*
 * Índice de la tarea a la que hay que cambiar en el próximo retorno
 * de IRQ. -1 significa "no cambiar".
 *
 * Usado por el stub de IRQ. El handler C lo actualiza, el assembly
 * lo consulta.
 */
extern int g_preempt_next_idx;

/*
 * Maneja el tick del timer desde el handler de IRQ.
 * Decide si cambiar de tarea y actualiza g_preempt_next_idx.
 */
void task_tick_from_irq(void);

/*
 * Devuelve el contexto de la tarea indicada (para el assembly).
 */
task_context_t *task_context_of(int idx);

/*
 * Actualiza g_current al índice dado.
 */
void task_set_current(int idx);

